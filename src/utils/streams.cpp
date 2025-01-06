// we should use malloc to define arbitarty streams but lets just use a buffer
// for now. Needs revisiting when we have multiple processes.

// TODO put mutexs in there own file

typedef char Mutex;  // maybe better as a struct?

typedef struct Stream {
  int count;
  Mutex message;
  Mutex slot;
  char queue[10];
} Stream;

Stream STDIN = {0, 0, 1, {'\0'}};

// RACE condition? what if 2 processes are waiting at once
// need to use a counting semaphore?
void wait(Mutex* mutex) {
  for (;;) {
    if (*mutex) {
      *mutex = 0;
      break;
    }
  }
}

void signal(Mutex* mutex) {
  *mutex = 1;
}

char stdin_get() {
  wait(&STDIN.message);
  char cout = STDIN.queue[0];
  for (int i = 0; i < STDIN.count - 1; i++) {
    STDIN.queue[i] = STDIN.queue[i + 1];
  }
  STDIN.queue[STDIN.count] = '\0';
  STDIN.count--;
  signal(&STDIN.slot);
  return cout;
}

void stdin_put(char c) {
  wait(&STDIN.slot);
  if (STDIN.count >= 10) {
    return;
  }
  STDIN.queue[STDIN.count] = c;
  STDIN.count++;
  signal(&STDIN.message);
}
