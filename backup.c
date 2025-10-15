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

typedef struct {
  char *name;
  TodoItem *head;
  bool collapsed;
} TodoGroup;

typedef enum {
  TODO,
  MENU,
  ADD,
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
  if(group->head == NULL) {
    group->head = newItem;
    return;
  } 
  
  TodoItem *temp = group->head;
  while(temp->next != NULL) {
    temp = temp->next;
  } temp->next = newItem;
}

void TodoApp() {
  bool running = true;
  int maxX, maxY;
  getmaxyx(stdscr, maxY, maxX);
  int aCurX = 2;
  WINDOW *mainWindow = newwin(maxY, maxX, 0, 0);
  WINDOW *subWin1 = newwin(3, maxX - 2, maxY - 4, 1);

  PANEL *panel1 = new_panel(mainWindow);
  PANEL *panel2 = new_panel(subWin1); // bottom menu

  todoMode mode = TODO; // 0 - MenuMode : 1 - TodoList Mode
  
  // *AUTOMATE THIS SHIT PLS
  TodoGroup group1 = {"MAJORS", NULL, false};
  TodoGroup group2 = {"MINORS", NULL, false};
  TodoGroup group3 = {"TODOLIST", NULL, false};

  addTodo(&group1, "CMSC 12 exer5");
  addTodo(&group1, "CMSC 56 predickate");
  addTodo(&group1, "MATH 27 exer 3");
  addTodo(&group2, "STS Activity 4");
  addTodo(&group2, "HK Script");
  addTodo(&group3, "Add ticking loop");
  addTodo(&group3, "File I/O");

  TodoGroup groups[32] = {group1, group2, group3};
  int groupCount = (int)(sizeof(groups) / sizeof(groups[0]));

  int highlight = 0;
  int input = 0;
  int a = 0;
  char addBuffer[128];

  keypad(mainWindow, true);
  top_panel(panel1);

  doupdate();
  // MAIN TODO LOOP
  while(running) {
    if (mode == TODO) { input = wgetch(mainWindow); }
    else if(mode == TODO) { input = wgetch(subWin1); }
    werase(mainWindow);
    box(mainWindow, 0, 0);

    //INPUT
    if(mode == TODO) {
      switch(input) {
        case 'a':
          mode = ADD;
          continue;
        case KEY_UP:
          if(highlight <= 0) { highlight = a - 1; } else { highlight--; }
          // mvwprintw(mainWindow, 10, 50, "a:%d, highlight:%d", a, highlight);
          break;
        case KEY_DOWN:
          if(highlight >= a - 1) { highlight = 0; } else { highlight++; }
          // mvwprintw(mainWindow, 10, 50, "a:%d , highlight:%d", a, highlight);
          break;
        case 27: 
          mode = MENU;
          continue;
          break;
        case 10:
          break;
      }
    } 
    else if(mode == ADD) {
      aCurX = 0;
      keypad(subWin1, TRUE);
      curs_set(1);
      top_panel(panel2);
      wmove(subWin1, 1, 2);
      werase(subWin1);
      box(subWin1, 0, 0);
      mvwprintw(subWin1, 0, 2, "Add Item");
      mvwprintw(subWin1, 1, 1, ":");

      for(int i = 0; i < (int)(sizeof(addBuffer)); i++) {
        addBuffer[i] = '\0';
      }

      while((input = wgetch(subWin1)) != 27) {
        box(subWin1, 0, 0);
        mvwprintw(subWin1, 0, 2, "Add Item");
        mvwprintw(subWin1, 1, 1, ":");

        if(isprint(input) && aCurX < 127) {
          wmove(subWin1, 1, aCurX);
          addBuffer[aCurX] = input;
          aCurX++;
        } else if((input == 263 || input == KEY_BACKSPACE) && aCurX > 0) {
          aCurX--;
          wmove(subWin1, 1, aCurX);
          addBuffer[aCurX] = '\0';
          mvwhline(subWin1, 1, 2, ' ', maxX - 4);
        }
        mvwprintw(subWin1, 1, 2, "%s", addBuffer);
        /* 
        if(aCurX > 0) {
          for(int i = 0; i < aCurX; i++) {
            mvwprintw(subWin1, 1, i + 2, "%c", addBuffer[i]);
          }
        } */
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

    // RENDER TODOLIST
    int printY = 0;
    a = 0;
    for(int i = 0; groups[i].name != NULL; i++) {
      if(printY == highlight && mode == TODO && groups[i].collapsed == true) {
        wattron(mainWindow, A_BOLD);
        wattron(mainWindow, A_STANDOUT);
        mvwprintw(mainWindow, 1 + printY, 1, "> %s", groups[i].name);
        wattroff(mainWindow, A_BOLD);
        wattroff(mainWindow, A_STANDOUT);
      } else if(printY == highlight && mode == TODO && groups[i].collapsed == false) {
        wattron(mainWindow, A_BOLD);
        wattron(mainWindow, A_STANDOUT);
        mvwprintw(mainWindow, 1 + printY, 1, "V %s", groups[i].name);
        wattroff(mainWindow, A_BOLD);
        wattroff(mainWindow, A_STANDOUT);
      } else if(printY == highlight && mode != TODO) {
        wattron(mainWindow, A_BOLD);
        wattron(mainWindow, A_UNDERLINE);
        if(groups[i].collapsed == false) {
          mvwprintw(mainWindow, 1 + printY, 1, "V %s", groups[i].name);
        } else {
          mvwprintw(mainWindow, 1 + printY, 1, "> %s", groups[i].name);
        }
        wattroff(mainWindow, A_BOLD);
        wattroff(mainWindow, A_UNDERLINE);
      } else {
        wattron(mainWindow, A_BOLD);
        if(groups[i].collapsed == false) {
          mvwprintw(mainWindow, 1 + printY, 1, "V %s", groups[i].name);
        } else {
          mvwprintw(mainWindow, 1 + printY, 1, "> %s", groups[i].name);
        }
        wattroff(mainWindow, A_BOLD); } 
      printY++;
      a++;
                
      TodoItem *temp = groups[i].head; 
      while(temp != NULL) {
        if(printY == highlight && temp->completed == false) {
          wattron(mainWindow, A_STANDOUT);
          mvwprintw(mainWindow, 1 + printY, 5, "%s", temp->title);
          wattroff(mainWindow, A_STANDOUT);
        } else if (printY == highlight && temp->completed == false && mode != TODO) {
          wattron(mainWindow, A_UNDERLINE);
          mvwprintw(mainWindow, 1 + printY, 5, "%s", temp->title);
          wattron(mainWindow, A_UNDERLINE);
        }else {
          mvwprintw(mainWindow, 1 + printY, 5, "%s", temp->title); }
          printY++;
          a++;
          temp = temp->next;
        }
    } 
    
    update_panels();
    doupdate();
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
    highlightWindow(window);
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

