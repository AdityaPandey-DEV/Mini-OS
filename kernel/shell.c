#include "shell.h"
#include "vga.h"
#include "lib.h"
#include "timer.h"
#include "fs.h"

#define MAX_LINE 128
#define MAX_HISTORY 16

static char line[MAX_LINE];
static int cursor = 0;
static char history[MAX_HISTORY][MAX_LINE];
static int hist_count = 0;
static int hist_pos = -1;

static const char* cmds[] = {"help","echo","clear","about","mem","time","uptime","sysinfo","ls","cat"};

static void prompt() {
    vga_write("minios> ");
}

static void newline() { vga_putc('\n'); }

static void print_help() {
    vga_write("Commands:\n");
    for (unsigned i=0;i<sizeof(cmds)/sizeof(cmds[0]);i++){ vga_write("  "); vga_write(cmds[i]); vga_putc('\n'); }
}

static void exec_cmd(const char* cmdline) {
    // Save to history
    if (strlen(cmdline) > 0) {
        if (hist_count < MAX_HISTORY) {
            strcpy(history[hist_count++], cmdline);
        } else {
            for (int i=1;i<MAX_HISTORY;i++) strcpy(history[i-1], history[i]);
            strcpy(history[MAX_HISTORY-1], cmdline);
        }
    }
    hist_pos = -1;

    // Parse
    char buf[MAX_LINE]; strcpy(buf, cmdline);
    char* p = buf; while(*p && isspace(*p)) p++;
    char* cmd = p;
    while(*p && !isspace(*p)) p++;
    if (*p) *p++='\0';
    while(*p && isspace(*p)) p++;
    char* args = p;

    if (strcmp(cmd, "help") == 0) {
        print_help();
    } else if (strcmp(cmd, "echo") == 0) {
        vga_write(args);
        newline();
    } else if (strcmp(cmd, "clear") == 0) {
        vga_clear();
    } else if (strcmp(cmd, "about") == 0) {
        vga_write("MiniOS: tiny 32-bit kernel with web terminal demo.\n");
    } else if (strcmp(cmd, "mem") == 0) {
        vga_write("Mem: demo only.\n");
    } else if (strcmp(cmd, "time") == 0) {
        uint64_t ms = timer_uptime_ms();
        char tmp[32];
        // very small itoa
        int i=0; uint64_t t=ms; char rev[32]; if(t==0) rev[i++]='0'; while(t){ rev[i++]='0'+(t%10); t/=10; }
        int j=0; while(i--) tmp[j++]=rev[i]; tmp[j]=0;
        vga_write(tmp); vga_write(" ms\n");
    } else if (strcmp(cmd, "uptime") == 0) {
        uint64_t ms = timer_uptime_ms();
        uint64_t s = ms/1000ULL; uint64_t m = s/60ULL; uint64_t h = m/60ULL;
        s%=60; m%=60;
        // crude print
        char bufh[32]; char bufm[32]; char bufs[32];
        int i; uint64_t t; char r[32]; int k;
        k=0; t=h; if(t==0) r[k++]='0'; while(t){ r[k++]='0'+(t%10); t/=10; } i=0; while(k--) bufh[i++]=r[k]; bufh[i]=0;
        k=0; t=m; if(t==0) r[k++]='0'; while(t){ r[k++]='0'+(t%10); t/=10; } i=0; while(k--) bufm[i++]=r[k]; bufm[i]=0;
        k=0; t=s; if(t==0) r[k++]='0'; while(t){ r[k++]='0'+(t%10); t/=10; } i=0; while(k--) bufs[i++]=r[k]; bufs[i]=0;
        vga_write(bufh); vga_write("h "); vga_write(bufm); vga_write("m "); vga_write(bufs); vga_write("s\n");
    } else if (strcmp(cmd, "sysinfo") == 0) {
        vga_write("CPU: 32-bit protected mode\nPIC: remapped, PIT 100Hz\n");
    } else if (strcmp(cmd, "ls") == 0) {
        size_t n; const struct file* f = fs_list(&n);
        for (size_t i=0;i<n;i++){ vga_write(f[i].name); vga_putc('\n'); }
    } else if (strcmp(cmd, "cat") == 0) {
        const char* name = args;
        if (!*name){ vga_write("Usage: cat <FILE>\n"); }
        else { const char* d = fs_read(name); if (d) vga_write(d); else vga_write("Not found\n"); }
    } else if (*cmd) {
        vga_write("Unknown: "); vga_write(cmd); vga_putc('\n');
    }
}

static void redraw_line() {
    // naive: new line
    vga_putc('\r');
    // Move to line start by printing CR then reprinting prompt and buffer
    // (our VGA has no direct read; assume cursor is at line start)
    vga_write("minios> ");
    vga_write(line);
}

static void accept_line() {
    line[cursor] = 0;
    vga_putc('\n');
    exec_cmd(line);
    cursor = 0; line[0]=0;
    prompt();
}

static void history_up() {
    if (hist_count==0) return;
    if (hist_pos+1 < hist_count) hist_pos++;
    strcpy(line, history[hist_count-1-hist_pos]);
    cursor = (int)strlen(line);
    redraw_line();
}

static void history_down() {
    if (hist_pos <= 0) { hist_pos=-1; cursor=0; line[0]=0; redraw_line(); return; }
    hist_pos--;
    strcpy(line, history[hist_count-1-hist_pos]);
    cursor = (int)strlen(line);
    redraw_line();
}

static void autocomplete() {
    // simple: find first command starting with current line
    for (unsigned i=0;i<sizeof(cmds)/sizeof(cmds[0]);i++){
        if (strncmp(cmds[i], line, strlen(line))==0){
            strcpy(line, cmds[i]); cursor=(int)strlen(line); redraw_line(); return;
        }
    }
}

void shell_init(void) {
    cursor = 0; line[0]=0; hist_count=0; hist_pos=-1;
    vga_write_color("Welcome to MiniOS! Type 'help'.\n", 0x0A);
    prompt();
}

void shell_input(char c) {
    if (c == '\n') { accept_line(); return; }
    if (c == '\b') { if (cursor>0){ cursor--; line[cursor]=0; vga_write("\b \b"); } return; }
    if (c == '\t') { autocomplete(); return; }
    // rudimentary arrow keys via escape sequences not implemented; history mapped on PageUp/PageDown not available
    if (cursor < MAX_LINE-1 && c >= 32 && c < 127) {
        line[cursor++] = c; line[cursor]=0; vga_putc(c);
    }
}

