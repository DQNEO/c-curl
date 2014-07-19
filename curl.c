/*
 * Simple curl application to do HTTP GET
 */
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#define MAX_BUF 65536

char wr_buf[MAX_BUF+1];
int  wr_index;

/*
 * callback function (called within the context of curl_easy_perform.
 */
size_t mycallback( void *buffer, size_t size, size_t nmemb, void *userp )
{
    int segsize = size * nmemb;

    /* Check to see if this data exceeds the size of our buffer. If so,
     * set the user-defined context value and return 0 to indicate a
     * problem to curl.
     */
    if (wr_index + segsize > MAX_BUF) {
        *(int *)userp = 1;
        return 0;
    }

    /* Copy the data from the curl buffer into our buffer */
    memcpy((void *)&wr_buf[wr_index], buffer, (size_t)segsize);

    wr_index += segsize;     /* Update the write index */
    wr_buf[wr_index] = '\0'; /* Null terminate the buffer */

    return segsize;     /* Return the number of bytes received, indicating to curl that all is okay */
}

int main(int argc, char *argv[])
{
    char *url;
    CURL *curl;
    CURLcode ret;
    int  wr_error;

    if (argc < 2) {
        fprintf(stderr, "argument not given\n");
        return 1;
    }

    url = argv[1];

    wr_error = 0;
    wr_index = 0;

    /* First step, init curl */
    curl = curl_easy_init();
    if (!curl) {
        printf("couldn't init curl\n");
        return 0;
    }

    /* Tell curl the URL of the file we're going to retrieve */
    curl_easy_setopt(curl, CURLOPT_URL, url);

    /* Tell curl that we'll receive data to the function mycallback, and
     * also provide it with a context pointer for our error return.
     */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&wr_error);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, mycallback);

    /* Allow curl to perform the action */
    ret = curl_easy_perform(curl);

    if (ret == 0) {
        /* Emit the page if curl indicates that no errors occurred */
        printf("%s\n", wr_buf);
    } else {
        printf("ret = %d (write_error = %d)\n", ret, wr_error);
    }

    curl_easy_cleanup(curl);
    return 0;
}
