/* simple CoAP server for testing purpose */

#include <netdb.h>
#include <stdio.h>
#include <coap/coap.h>

/* creating the default response for the test */
static void response_msg(coap_context_t *ctx,
                         struct coap_resource_t *resource,
                         const coap_endpoint_t *local_interface,
                         coap_address_t *peer,
                         coap_pdu_t *request,
                         str *token,
                         coap_pdu_t *response) {

    const char* msg = "The CoAP Testserver is alive and reachable!";
    unsigned char buf[3];

    response->hdr->code = COAP_RESPONSE_CODE(205);

    // The response message is plain text.
    coap_add_option(response,
                    COAP_OPTION_CONTENT_TYPE,
                    coap_encode_var_bytes(buf,
                                          COAP_MEDIATYPE_TEXT_PLAIN),
                    buf);

    // Adding the maxage to the response.
    coap_add_option(response,
                    COAP_OPTION_MAXAGE,
                    coap_encode_var_bytes(buf, 0x2ffff),
                    buf);

    // Adding the text data to the message.
    coap_add_data(response,
                  strlen(msg),
                  (unsigned char *)msg);
}

/* the main loop */
int main(int argc, char* argv[]){
    /* The global state of the CoAP stack. */
    coap_context_t *ctx = NULL;
    /* union with the address struct's */
    coap_address_t server_addr;
    /* Hash table for list of known resources. */
    coap_resource_t *index = NULL;
    /* File descriptor to be watched. */
    fd_set readfds;

    /**********************************
     * Prepare the CoAP server socket *
     **********************************/

    /* Initialize coap_address_t object addr to default values. */
    coap_address_init(&server_addr);

    server_addr.addr.sin.sin_family = AF_INET|AF_INET6;       // listen v4 and v6
    server_addr.addr.sin.sin_addr.s_addr = INADDR_LOOPBACK;   // only on localhost
    server_addr.addr.sin.sin_port = htons(COAP_DEFAULT_PORT); // defining the port

    ctx = coap_new_context(&server_addr);
    if (!ctx)
        exit(EXIT_FAILURE);

    /* Initialize the CoAP index resourse. */
    index = coap_resource_init(NULL, 0, 0);

    /* Adding one handler.
     * We just need one handler for this simple test. */
    coap_register_handler(index, COAP_REQUEST_GET, response_msg);

    /* Adding the handler to the index. */
    coap_add_resource(ctx, index);

    /* Now starting the server. */
    while (1) {
        FD_ZERO(&readfds);
        FD_SET( ctx->sockfd, &readfds );
        /* Reading out from the socket ... */
        int result = select(FD_SETSIZE, &readfds, 0, 0, NULL);
        if (result < 0) {      // check for errors
            perror("error while select on socket");
            exit(EXIT_FAILURE);
        }
        else if (result > 0) { // read from socket
            if (FD_ISSET(ctx->sockfd, &readfds))
                coap_read(ctx);
        }
    }
}
