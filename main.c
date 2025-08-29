#include <stdio.h>
#include <ncurses.h>
#include <stdbool.h>
#include <string.h>

typedef struct { 
    int id;
    char * title;
    char * data;
} Note;

typedef struct {
    Note notes[4];
} NoteGroup;

void noteEdit (WINDOW *window, Note note, char *textBuffer) {
    //load to buffer
    //display buffer
    //move cursor sa dulo ng text
    //get input
        //buffer + input
        //display buffer / addch
        //move cursor

    //REMAKE THE ENTIRE FUCKING "TEXTBOX"
    char linedTextBuffer[512][1024];
    int j = 0;
    int k = 1;
    int l = 0;
    int m = 0;
    int ch;
    for(int i = 0; i < strlen(note.data); i++) {
        textBuffer[i] = note.data[i];
        l++;
        j++;
    } 
    textBuffer[l] = '\0';
    box(window, 0, 0);
    mvwprintw(window, 1, 1, "%s", textBuffer);
    keypad(window, true);

    while((ch = wgetch(window)) != 27) {
        werase(window);
        box(window, 0, 0);

        memset(linedTextBuffer, 0, sizeof(linedTextBuffer));

        if(ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            j--;
            l--;
            if(l > 0) { textBuffer[l] = '\0'; }
            //wmove(window, k, j); 
        } else if((ch == KEY_ENTER || ch == '\n')) {
            textBuffer[j] = '\n';
            k++;
            j = 1;
            //wmove(window, k, j); 
        } else {  
            textBuffer[l] = (char)ch;
            j++;
            l++;
            if(l > 0) { textBuffer[l] = '\0'; }
            //wmove(window, k, j); 
        }

        // read every character until '\n'
        // store 

        int n = 0;
        int o = 0;
        for(int i = 0; i < strlen(textBuffer); i++) {
            if(textBuffer[i] == '\n') {
                linedTextBuffer[n][o] = '\0';
                o = 0;
                n++;
            } else {
                linedTextBuffer[n][o] = textBuffer[i];
                o++;
            }
        } linedTextBuffer[n][o] = '\0';

        for(int i = 0; i <= n; i++) {
            mvwprintw(window, 1 + i, 1, "%s", linedTextBuffer[i]);
        } wmove(window, n + 1, o + 1);
    }

    keypad(window, false);
}

void noteUpdate(Note n, WINDOW *win) {

}

void enableTyping(WINDOW *window, int y, int x) {
    echo();
    curs_set(2);
    wmove(window, y, x);
}

void disableTyping() {
    noecho();
    curs_set(0);
}

int main () {
    initscr();
    noecho();
    curs_set(0);
    refresh();

    int Xmax, Ymax;
    bool running = true;

    getmaxyx(stdscr, Ymax, Xmax);

    WINDOW *optionWindow = newwin(Ymax, Xmax/4, 0, 0);
    WINDOW *contentWindow = newwin(Ymax, (3 * Xmax)/4, 0, Xmax/4);
    char textBuffer[1024];

    box(optionWindow, 0, 0);
    box(contentWindow, 0, 0);

    wrefresh(optionWindow);
    wrefresh(contentWindow);

    Note note1 = {0, "note1", "haha!"};
    Note note2 = {1, "note2", "hahaasdfasdfadsf"};
    Note note3 = {2, "note3", "dafsdfadfa!"};
    Note note4 = {3, "note4", "asdfadsg fdasdfasdfa sdfadfasdfasfol"};

    NoteGroup ng = {note1, note2, note3, note4};

    int highlight = 0;
    int choice;

    keypad(optionWindow,TRUE);

    while(running) {
        werase(optionWindow);

        box(optionWindow, 0, 0);
        box(contentWindow, 0, 0);

        for (int i = 0; i < (int)(sizeof(ng.notes) / sizeof(ng.notes[0])); i++) {
            if(highlight == i) {
                wattron(optionWindow, A_REVERSE);
                mvwprintw(optionWindow, 1+i, 1, "%d | %s", ng.notes[i].id ,ng.notes[i].title);
                //mvwprintw(optionWindow, i+1, 1, "betlog");
                wattroff(optionWindow, A_REVERSE);
            } else {
                mvwprintw(optionWindow, 1+i, 1, "%d | %s", ng.notes[i].id ,ng.notes[i].title);
            }
        }

        wrefresh(optionWindow);
        wrefresh(contentWindow);

        choice = wgetch(optionWindow);
        switch(choice) {
            case KEY_UP:
                highlight--;
                if(highlight < 0) {
                    highlight = (int)(sizeof(ng) / sizeof(ng.notes[0]) - 1);
                }
                break;
            case KEY_DOWN:
                highlight++;
                if(highlight > (int)(sizeof(ng) / sizeof(ng.notes[0])) - 1) {
                    highlight = 0;
                }
                break;
            case 10: 
                keypad(optionWindow, false);
                werase(contentWindow);
                enableTyping(contentWindow, 1, 1);
                noteEdit(contentWindow, ng.notes[highlight], textBuffer);
                disableTyping();
                keypad(optionWindow, true);
                break;
            default:
                break;
        }
    }

    getch();
    endwin();
    return 0;
}
