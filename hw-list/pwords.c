/*
 * Word count application with one thread per input file.
 *
 * You may modify this file in any way you like, and are expected to modify it.
 * Your solution must read each input file from a separate thread. We encourage
 * you to make as few changes as necessary.
 */

/*
 * Copyright © 2021 University of California, Berkeley
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#include "word_count.h"
#include "word_helpers.h"

struct args {
  word_count_list_t* wclist;
  FILE* infile;
};

void* count_words_wrapper(void* arg) {
  struct args* args = (struct args*) arg;
  word_count_list_t* wclist = args->wclist;
  FILE* infile = args->infile;
  count_words(wclist, infile);
  return NULL;
}
/*
 * main - handle command line, spawning one thread per file.
 */
int main(int argc, char* argv[]) {
  /* Create the empty data structure. */
  word_count_list_t word_counts;
  init_words(&word_counts);

  if (argc <= 1) {
    /* Process stdin in a single thread. */
    count_words(&word_counts, stdin);
  } else {
    /* TODO */
    struct args args[argc - 1];
    for(int i = 1; i < argc; i++) {
      FILE* infile = fopen(argv[i], "r");
      if(infile == NULL) {
        printf("File %s does not exist\n", argv[i]);
        return 1;
      }
      
      args[i-1].wclist = &word_counts;
      args[i-1].infile = infile;
    }
    int num_threads = argc - 1;
    

    pthread_t threads[num_threads];
    for (int i = 0; i < num_threads; i++) {
      pthread_create(&threads[i], NULL, count_words_wrapper, &args[i]);
    }
    // wait for all threads to finish 
    for (int i = 0; i < num_threads; i++) {
      pthread_join(threads[i], NULL);
    }
  }

  /* Output final result of all threads' work. */
  wordcount_sort(&word_counts, less_count);
  fprint_words(&word_counts, stdout);
  return 0;
}
