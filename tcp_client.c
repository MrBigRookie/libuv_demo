/*
 * @Author: wz
 * @Date: 2024-10-30 15:46:23
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

typedef struct
{
    uv_tcp_t socket;
    uv_connect_t connect_req;
    uv_buf_t send_buffer;
    uv_timer_t timer;
} client_t;

void on_write(uv_write_t *req, int status)
{
    if (status)
    {
        fprintf(stderr, "Write error: %s\n", uv_strerror(status));
    }
    free(req);
}

void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    if (nread > 0)
    {
        // printf("Received: %.*s\n", (int)nread, buf->base);
        printf("Received: %s\n", buf->base);
    }
    else if (nread < 0)
    {
        if (nread != UV_EOF)
        {
            fprintf(stderr, "Read error: %s\n", uv_strerror(nread));
        }
        uv_close((uv_handle_t *)stream, NULL);
    }
    free(buf->base);
}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    buf->base = calloc(suggested_size, 1);
    buf->len = suggested_size;
}

void on_timer(uv_timer_t *handle)
{
    client_t *client = (client_t *)handle->data;

    // Send a message to the server
    const char *message = "Hello, server!";
    client->send_buffer = uv_buf_init((char *)message, strlen(message));
    uv_write_t *write_req = malloc(sizeof(uv_write_t));
    uv_write(write_req, (uv_stream_t *)&client->socket, &client->send_buffer, 1, on_write);
}

void on_connect(uv_connect_t *req, int status)
{
    if (status < 0)
    {
        fprintf(stderr, "Connect error: %s\n", uv_strerror(status));
        return;
    }

    client_t *client = req->data;

    // Start reading from the server
    uv_read_start((uv_stream_t *)&client->socket, alloc_buffer, on_read);

    uv_timer_init(uv_default_loop(), &client->timer);
    client->timer.data = client;
    uv_timer_start(&client->timer, on_timer, 1000, 1000);
}

int main()
{
    uv_loop_t *loop = uv_default_loop();

    client_t *client = malloc(sizeof(client_t));
    uv_tcp_init(loop, &client->socket);

    struct sockaddr_in dest;
    uv_ip4_addr("127.0.0.1", 9999, &dest);

    client->connect_req.data = client;
    uv_tcp_connect(&client->connect_req, &client->socket, (const struct sockaddr *)&dest, on_connect);

    return uv_run(loop, UV_RUN_DEFAULT);
}
