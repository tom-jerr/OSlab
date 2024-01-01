/*
 * Implementation of the word_count interface using Pintos lists and pthreads.
 *
 * You may modify this file, and are expected to modify it.
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

#ifndef PINTOS_LIST
#error "PINTOS_LIST must be #define'd when compiling word_count_lp.c"
#endif

#ifndef PTHREADS
#error "PTHREADS must be #define'd when compiling word_count_lp.c"
#endif

#include "word_count.h"

void init_words(word_count_list_t* wclist) { /* TODO */
  if(wclist == NULL) {
    printf("wclist is NULL\n");
    return;
  }
  list_init(wclist);                      
}

size_t len_words(word_count_list_t* wclist) {
  /* TODO */
  pthread_mutex_lock(&wclist->lock);
  if (wclist == NULL) return 0;
  size_t len = list_size(wclist);
  pthread_mutex_unlock(&wclist->lock);
  return len;
}

word_count_t* find_word(word_count_list_t* wclist, char* word) {
  /* TODO */
  if (wclist == NULL || word == NULL) {
    // pthread_mutex_unlock(&wclist->lock);
    return NULL;
  }
  struct list_elem* e;
  for(e = list_begin(wclist); e != list_tail(wclist); e = list_next(e)) {
    word_count_t* wc = list_entry(e, word_count_t, elem);
    if(strcmp(wc->word, word) == 0) {
      // pthread_mutex_unlock(&wclist->lock);
      return wc;
    }
  }
  // pthread_mutex_unlock(&wclist->lock);
  return NULL;
}

word_count_t* add_word(word_count_list_t* wclist, char* word) {
  /* TODO */
  if (word == NULL) return NULL;
  word_count_t* wc = NULL;
  // find_words前已经获取到锁
  pthread_mutex_lock(&wclist->lock);
  wc = find_word(wclist, word);
  // if word is already in the list, increment the count
  if(wc != NULL) {
    // pthread_mutex_lock(&wclist->lock);
    wc->count += 1;
    pthread_mutex_unlock(&wclist->lock);
    return wc;
  } else {
    // if word is not in the list, add it to the front of the list
    // 元素和元素的名称均需要动态分配空间
    // pthread_mutex_lock(&wclist->lock);
    wc = (word_count_t *) malloc(sizeof(word_count_t));
    wc->word = (char *) malloc(strlen(word) + 1);
    strcpy(wc->word, word);
    wc->count = 1;
    list_push_front(wclist, &(wc->elem));
    pthread_mutex_unlock(&wclist->lock);
    return wc;
  }
}

void fprint_words(word_count_list_t* wclist, FILE* outfile) { /* TODO */
  if(outfile == NULL ||wclist == NULL) {
    return;
  }
  struct list_elem* e;
  for(e = list_begin(wclist); e != list_tail(wclist); e = list_next(e)) {
    word_count_t* wc = list_entry(e, word_count_t, elem);
    fprintf(outfile, "%8d\t%s\n", wc->count, wc->word);
  }
}

static bool less_list(const struct list_elem* ewc1, const struct list_elem* ewc2, void* aux) {
  /* TODO */
  if (ewc1 == NULL || ewc2 == NULL) return false;
  if (ewc1->prev == NULL || ewc2->prev == NULL \
      || ewc1->next == NULL || ewc2->next == NULL) return false;
  word_count_t* wc1 = list_entry(ewc1, word_count_t, elem);
  word_count_t* wc2 = list_entry(ewc2, word_count_t, elem);
  if (wc1 == NULL || wc2 == NULL) return false;
  if (wc1->count < wc2->count) return true;
  if (wc1->count == wc2->count) {
    if (strcmp(wc1->word, wc2->word) < 0) return true;
  }
  return false;
}

void wordcount_sort(word_count_list_t* wclist,
                    bool less(const word_count_t*, const word_count_t*)) {
  /* TODO */
  list_sort(&wclist->lst, less_list, NULL);
}
