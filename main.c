#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <panel.h>
#include <string.h>
#include <ctype.h>

void placeholderVoid () {

}

typedef struct {
  char *title;
  void (*run)(void);
} ApplicationItem;

typedef struct TodoItem {
  bool completed;
  char *title;
  char *description;
  struct TodoItem *next;
} TodoItem;

typedef struct TodoGroup {
  char *name;
  TodoItem *todoHead;
  struct TodoGroup *nextGroup;
  bool collapsed;
} TodoGroup;

typedef struct TodoPage{
  char *name;
  struct TodoPage *nextPage;
  TodoGroup *groupHead;
} TodoPage;

typedef enum {
  TODO,
  MENU,
  ADD,
  PAGE_SELECT,
} todoMode;

TodoItem *createTodo(char *_title) {
  TodoItem *newTodo = (TodoItem*)malloc(sizeof(TodoItem));
  newTodo->completed = false;
  newTodo->title = _title;
  newTodo->description = NULL;
  newTodo->next = NULL;
  return newTodo;
}

void addTodo(TodoGroup *group, char *_title) {
  TodoItem *newItem = createTodo(_title);
  if(group->todoHead == NULL) {
    group->todoHead = newItem;
    return;
  } 
  
  TodoItem *temp = group->todoHead;
  while(temp->next != NULL) {
    temp = temp->next;
  } temp->next = newItem;
}

TodoGroup* createTodoGroup(char *_name) {
  TodoGroup *newGroup = (TodoGroup*)malloc(sizeof(TodoGroup));
  newGroup->name = _name;
  newGroup->todoHead = NULL;
  newGroup->nextGroup = NULL;
  newGroup->collapsed = false;
  return newGroup;
}

void addGroup(char* _name, TodoPage* page) {
  TodoGroup* newGroup = createTodoGroup(_name);
  if(page->groupHead == NULL) {
    page->groupHead = newGroup;
    return;
  }

  TodoGroup* temp = page->groupHead;
  while(temp->nextGroup != NULL) {
    temp = temp->nextGroup;
  } temp->nextGroup = newGroup;
}

TodoPage* createPage(char* _name) {
  TodoPage* newPage = (TodoPage*)malloc(sizeof(TodoPage));
  newPage->name = _name;
  newPage->nextPage = NULL;
  newPage->groupHead = NULL;
  return newPage;
}

void addPage(char* _name) {
  TodoPage* newPage = createPage(_name);
  addGroup(newPage->name, newPage); 
}

void TodoApp() {
  bool running = true;
  int maxX, maxY;
  getmaxyx(stdscr, maxY, maxX); 
  int aCurX = 2;
  WINDOW *mainWindow = newwin(maxY, maxX, 0, 0);
  WINDOW *subWin1 = newwin(1, 1, 1, 1);

  PANEL *panel1 = new_panel(mainWindow);
  PANEL *panel2 = new_panel(subWin1); // bottom menu

  todoMode mode = TODO; // 0 - MenuMode : 1 - TodoList Mode
  
  // *AUTOMATE THIS SHIT PLS
  /*

  TodoGroup group1 = {"MAJORS", NULL, NULL, false};
  TodoGroup group2 = {"MINORS", NULL,  NULL, false};
  TodoGroup group3 = {"TODOLIST", NULL, NULL, false};

  group1.nextGroup = &group2;
  group2.nextGroup = &group3;

  addTodo(&group1, "CMSC 12 exer5");
  addTodo(&group1, "CMSC 56 predickate");
  addTodo(&group1, "MATH 27 exer 3");
  addTodo(&group2, "STS Activity 4");
  addTodo(&group2, "HK Script");
  addTodo(&group3, "Add ticking loop");
  addTodo(&group3, "File I/O");

  TodoGroup groups[32] = {group1, group2, group3};
  int groupCount = (int)(sizeof(groups) / sizeof(groups[0]));
  */

  //TodoPage page1 = {"PAGE 1", NULL, NULL};

  TodoPage* selectedPage = NULL; 
  TodoPage* headPage = NULL;

  int highlight = 0;
  int input = 0;
  int a = 0;
  char addBuffer[128];

  keypad(mainWindow, true);
  top_panel(panel1);

  doupdate();

  // MAIN TODO LOOP
  while(running) {
    // GRAPHICS
    werase(mainWindow);
    box(mainWindow, 0, 0);
    if(selectedPage != NULL) {
      char* titleDisplay = selectedPage->name;
      mvwprintw(mainWindow, 0, 1, "%s", titleDisplay);
    }
    
    // RENDER TODOLIST
    int printY = 0;
    a = 0;
    
    TodoGroup* currentGroup = NULL;
    if(selectedPage != NULL){ currentGroup = selectedPage->groupHead; }
    while(currentGroup != NULL) {
      if(printY == highlight && mode == TODO) {
        wattron(mainWindow, A_BOLD);
        wattron(mainWindow, A_STANDOUT);
        if(currentGroup->collapsed == false) {
          mvwprintw(mainWindow, 1 + printY, 1, "V %s", currentGroup->name); 
        } else if(!currentGroup->collapsed) {
          mvwprintw(mainWindow, 1 + printY, 1, "> %s", currentGroup->name);
        }
        wattroff(mainWindow, A_BOLD);
        wattroff(mainWindow, A_STANDOUT);
      } 
      else if(printY == highlight && mode != TODO) {
        wattron(mainWindow, A_BOLD);
        wattron(mainWindow, A_UNDERLINE);
        if(currentGroup->collapsed == false) {
          mvwprintw(mainWindow, 1 + printY, 1, "V %s", currentGroup->name);
        } else {
          mvwprintw(mainWindow, 1 + printY, 1, "> %s", currentGroup->name);
        }
        wattroff(mainWindow, A_BOLD);
        wattroff(mainWindow, A_UNDERLINE);
      } 
      else {
        wattron(mainWindow, A_BOLD);
        if(currentGroup->collapsed == false) {
          mvwprintw(mainWindow, 1 + printY, 1, "V %s", currentGroup->name);
        } else {
          mvwprintw(mainWindow, 1 + printY, 1, "> %s", currentGroup->name);
        }
        wattroff(mainWindow, A_BOLD); 
      } 
      printY++;
      a++;
     
      TodoItem *tempItem;
      if(currentGroup != NULL){ tempItem = currentGroup->todoHead; }
      while(tempItem != NULL) {
        if(printY == highlight && tempItem->completed == false) {
          wattron(mainWindow, A_STANDOUT);
          mvwprintw(mainWindow, 1 + printY, 5, "%s", tempItem->title);
          wattroff(mainWindow, A_STANDOUT);
        } 
        else if (printY == highlight && tempItem->completed == false && mode != TODO) {
          wattron(mainWindow, A_UNDERLINE);
          mvwprintw(mainWindow, 1 + printY, 5, "%s", tempItem->title);
          wattron(mainWindow, A_UNDERLINE);
        }
        else {
          mvwprintw(mainWindow, 1 + printY, 5, "%s", tempItem->title); 
        }
        printY++;
        a++;
        tempItem = tempItem->next;    
      }
      currentGroup = currentGroup->nextGroup;
    }
    update_panels();
    doupdate(); 

    //INPUT
    if (mode == TODO) { input = wgetch(mainWindow); }
    if(mode == TODO) {
      switch(input) {
        case 'a':
          mode = ADD;
        case KEY_UP:
          if(highlight <= 0) { highlight = a - 1; } else { highlight--; }
          break;
        case KEY_DOWN:
          if(highlight >= a - 1) { highlight = 0; } else { highlight++; }
          break;
        case 27: 
          mode = MENU;
          break;
        case 10:
          break;
      }
    } 
    else if(mode == ADD) {
      aCurX = 0;
      keypad(subWin1, TRUE);
      curs_set(1);

      move_panel(panel2, maxY - 5, 1);
      top_panel(panel2);
      werase(subWin1);
      wresize(subWin1, 4, 20);
      mvwprintw(subWin1, 1, 1, "[i] Add Item");
      mvwprintw(subWin1, 2, 1, "[g] Add Group");
      box(subWin1, 0, 0);

      bool a = false;
      bool b = false;
      char adding[32];

      
      for(int i = 0; i < (int)(sizeof(addBuffer)); i++) {
        addBuffer[i] = '\0';
      }

      while((input = wgetch(subWin1))) {
        if(!a) {
          if(input == 'i' || input == '1') {
            strcpy(adding, "Item");
            werase(subWin1);
            a = true;
            input = 0;
            goto addingit;
          } 
          else if(input == 'g' || input == '2') {
            strcpy(adding, "Group");
            werase(subWin1);
            a = true;
            input = 0;
            goto addingit;
          }
          else if(input == 27) {
            werase(subWin1);
            curs_set(false);
            keypad(subWin1, FALSE);
            mode = TODO;
            break;
          }
        } 
        else {
        addingit:
          if(!b) {
            werase(subWin1);
            wmove(subWin1, 1, 2);
            mvwprintw(subWin1, 0, 2, "Add %s", adding);
            mvwprintw(subWin1, 1, 1, ":");
            move_panel(panel2, maxY - 4, 1);
            wresize(subWin1, 3, maxX - 2); 
          }
          werase(subWin1);
          box(subWin1, 0, 0);
          mvwprintw(subWin1, 0, 2, "Add %s", adding);
          mvwprintw(subWin1, 1, 1, ":");

          if(isprint(input) && aCurX < 127) {
            wmove(subWin1, 1, aCurX);
            addBuffer[aCurX] = input;
            aCurX++;
          } 
          else if((input == 263 || input == KEY_BACKSPACE) && aCurX > 0) {
            aCurX--;
            wmove(subWin1, 1, aCurX);
            addBuffer[aCurX] = '\0';
            mvwhline(subWin1, 1, 2, ' ', maxX - 4);
          } 
          else if(input == 27) {
            werase(subWin1);
            curs_set(false);
            keypad(subWin1, FALSE);
            mode = TODO;
            break;
          } 
          else if(input == 343 && strlen(addBuffer) > 0) {
            if(adding == "Item") {
            } 
            else if(adding == "Group") {

            }
          }
          mvwprintw(subWin1, 1, 2, "%s", addBuffer);
        }
      }
      werase(subWin1);
      curs_set(false);
      keypad(subWin1, FALSE);
      mode = TODO;
    } 
    else if(mode == MENU) {
      if(input == 27) {
        running = false;
      }
    } 
}
}
void highlightWindow(WINDOW* win) {
  init_pair(1, COLOR_RED, -1);

  wattron(win, COLOR_PAIR(1));
  box(win, 0, 0);
  wattroff(win, COLOR_PAIR(1));
}

void MainMenu() {
  bool running = true;
  int w = 50;
  int h = 20;
  int maxX, maxY;
  char *title = "C TERMINAL PRODUCTIVITY APP";
  int titleLength = (int)strlen(title);

  ApplicationItem todoApp = {"Todo List (*WORK IN PROGRESS)", TodoApp};
  ApplicationItem noteTakingApp = {"Notes (NOT IMPLEMENTED)", placeholderVoid};
  ApplicationItem quizApp = {"Quiz (NOT IMPLEMENTED)", placeholderVoid};
  ApplicationItem appList[] = {todoApp, noteTakingApp, quizApp};
  int appListLength = (int)(sizeof(appList) / sizeof(appList[0]));

  getmaxyx(stdscr, maxY, maxX);

  WINDOW* window = newwin(h, w, (maxY - h) / 2, (maxX - w) / 2);
  int highlight = 0;
  int choice = 0;

  keypad(window, TRUE);
  while(running) {
    werase(window);
    //highlightWindow(window);
    box(window, 0, 0);
    mvwprintw(window, 0, (w - titleLength) / 2, "%s", title);
    for(int i = 0; i < appListLength; i++) {
      if(i == highlight) {
        wattron(window, A_STANDOUT);
        mvwprintw(window, i + 2, 1, "%s", appList[i].title);
        wattroff(window, A_STANDOUT);
      }
      else {
        mvwprintw(window, i + 2, 1, "%s", appList[i].title);
      }
    }
    
    int input = wgetch(window);
    switch(input) {
      case KEY_UP:
        if(highlight <= 0) { highlight = appListLength - 1; } else { highlight--;}
        break;
      case KEY_DOWN:
        if(highlight >= appListLength - 1) { highlight = 0; } else { highlight++; }
        break;
      case 10:
      case KEY_ENTER:
        choice = highlight;
        running = false;
        appList[choice].run();
        break;
    }
    
    wrefresh(window);
  }
  wrefresh(window);
  delwin(window);
  refresh();
  appList[choice].run();
}

int main() {
  initscr();
  start_color();
  use_default_colors();
  noecho();
  cbreak();
  curs_set(false);

  MainMenu();

  endwin();
  return 0;
}

