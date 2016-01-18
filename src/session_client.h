/**
 * \file session_client.h
 * \author Michal Vasko <mvasko@cesnet.cz>
 * \brief libnetconf2 session client manipulation
 *
 * Copyright (c) 2015 CESNET, z.s.p.o.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of the Company nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 */

#ifndef NC_SESSION_CLIENT_H_
#define NC_SESSION_CLIENT_H_

#include <libyang/libyang.h>

#ifdef ENABLE_SSH

#include <libssh/libssh.h>

#endif

#ifdef ENABLE_TLS

#include <openssl/ssl.h>

#endif

#include "session.h"
#include "netconf.h"
#include "messages_client.h"

/**
 * @brief Set location where libnetconf tries to search for YANG/YIN schemas.
 *
 * The location is search when connecting to a NETCONF server and building
 * YANG context for further processing of the NETCONF messages and data.
 *
 * Function is provided only via nc_client.h header file.
 *
 * @param[in] path Directory where to search for YANG/YIN schemas.
 * @return 0 on success, 1 on (memory allocation) failure.
 */
int nc_schema_searchpath(const char *path);

/**
 * @brief Connect to the NETCONF server via proviaded input/output file descriptors.
 *
 * Transport layer is supposed to be already set. Function do not cover authentication
 * or any other manipulation with the transport layer, it only establish NETCONF session
 * by sending and processing NETCONF \<hello\> messages.
 *
 * Function is provided only via nc_client.h header file.
 *
 * @param[in] fdin Input file descriptor for reading (clear) data from NETCONF server.
 * @param[in] fdout Output file descriptor for writing (clear) data for NETCONF server.
 * @param[in] ctx Optional parameter. If set, provides strict YANG context for the session
 *                (ignoring what is actually supported by the server side). If not set,
 *                YANG context is created for the session using \<get-schema\> (if supported
 *                by the server side) or/and by searching for YANG schemas in the searchpath
 *                (see nc_schema_searchpath()). In every case except not providing context
 *                to connect to a server supporting \<get-schema\> it is possible that
 *                the session context will not include all the models supported by the server.
 * @return Created NETCONF session object or NULL in case of error.
 */
struct nc_session *nc_connect_inout(int fdin, int fdout, struct ly_ctx *ctx);

#ifdef ENABLE_SSH

/**
 * @brief Destroy any dynamically allocated SSH-specific client context.
 *
 * Function is provided only via nc_client.h header file and only when libnetconf2 is compiled with libssh support.
 */
void nc_ssh_client_destroy(void);

/**
 * @brief Connect to the NETCONF server using SSH transport (via libssh).
 *
 * SSH session is created with default options. If the caller needs to use specific SSH session properties,
 * they are supposed to use nc_connect_libssh().
 *
 * Function is provided only via nc_client.h header file and only when libnetconf2 is compiled with libssh support.
 *
 * @param[in] host Hostname or address (both Ipv4 and IPv6 are accepted) of the target server.
 *                 'localhost' is used by default if NULL is specified.
 * @param[in] port Port number of the target server. Default value 830 is used if 0 is specified.
 * @param[in] username Name of the user to login to the server. The user running the application (detected from the
 *                     effective UID) is used if NULL is specified.
 * @param[in] ctx Optional parameter. If set, provides strict YANG context for the session
 *                (ignoring what is actually supported by the server side). If not set,
 *                YANG context is created for the session using \<get-schema\> (if supported
 *                by the server side) or/and by searching for YANG schemas in the searchpath
 *                (see nc_schema_searchpath()). In every case except not providing context
 *                to connect to a server supporting \<get-schema\> it is possible that
 *                the session context will not include all the models supported by the server.
 * @return Created NETCONF session object or NULL on error.
 */
struct nc_session *nc_connect_ssh(const char *host, uint16_t port, const char* username, struct ly_ctx *ctx);

/**
 * @brief Connect to the NETCONF server using the provided SSH (libssh) session.
 *
 * SSH session can have any options set, they will not be modified. If no options were set,
 * host 'localhost', port 22, and the username detected from the EUID is used. If socket is
 * set and connected only the host and the username must be set/is detected. Or the \p ssh_session
 * can already be authenticated in which case it is used directly.
 *
 * Function is provided only via nc_client.h header file and only when libnetconf2 is compiled with libssh support.
 *
 * @param[in] ssh_session libssh structure representing SSH session object. After passing it
 *                        to libnetconf2 this way, it is fully managed by it (including freeing!).
 * @param[in] ctx Optional parameter. If set, provides strict YANG context for the session
 *                (ignoring what is actually supported by the server side). If not set,
 *                YANG context is created for the session using \<get-schema\> (if supported
 *                by the server side) or/and by searching for YANG schemas in the searchpath
 *                (see nc_schema_searchpath()). In every case except not providing context
 *                to connect to a server supporting \<get-schema\> it is possible that
 *                the session context will not include all the models supported by the server.
 * @return Created NETCONF session object or NULL on error.
 */
struct nc_session *nc_connect_libssh(ssh_session ssh_session, struct ly_ctx *ctx);

/**
 * @brief Create another NETCONF session on existing SSH session using separated SSH channel.
 *
 * Function is provided only via nc_client.h header file and only when libnetconf2 is compiled with libssh support.
 *
 * @param[in] session Existing NETCONF session. The session has to be created on SSH transport layer using libssh -
 *                    it has to be created by nc_connect_ssh(), nc_connect_libssh() or nc_connect_ssh_channel().
 * @param[in] ctx Optional parameter. If set, provides strict YANG context for the session
 *                (ignoring what is actually supported by the server side). If not set,
 *                YANG context is created for the session using \<get-schema\> (if supported
 *                by the server side) or/and by searching for YANG schemas in the searchpath
 *                (see nc_schema_searchpath()). In every case except not providing context
 *                to connect to a server supporting \<get-schema\> it is possible that
 *                the session context will not include all the models supported by the server.
 * @return Created NETCONF session object or NULL on error.
 */
struct nc_session *nc_connect_ssh_channel(struct nc_session *session, struct ly_ctx *ctx);

/**
 * @brief Accept a Call Home SSH connection from a NETCONF server.
 *
 * Function is provided only via nc_client.h header file and only when libnetconf2 is compiled with libssh support.
 *
 * @param[in] port Port the NETCONF client will listen on.
 * @param[in] username Name of the user to login to the server. The user running the application (detected from the
 *                     effective UID) is used if NULL is specified.
 * @param[in] timeout Timeout for reading in milliseconds. Use negative value for infinite
 *                    waiting and 0 for immediate return if data are not available on wire.
 * @param[in] ctx Optional parameter. If set, provides strict YANG context for the session
 *                (ignoring what is actually supported by the server side). If not set,
 *                YANG context is created for the session using \<get-schema\> (if supported
 *                by the server side) or/and by searching for YANG schemas in the searchpath
 *                (see nc_schema_searchpath()). In every case except not providing context
 *                to connect to a server supporting \<get-schema\> it is possible that
 *                the session context will not include all the models supported by the server.
 * @return Created NETCONF session object or NULL on error.
 */
struct nc_session *nc_callhome_accept_ssh(uint16_t port, const char *username, int timeout, struct ly_ctx *ctx);

/**
 * @brief Add an SSH public and private key pair to be used for client authentication.
 *
 * Private key can be encrypted, the passphrase will be asked for before using it.
 *
 * Function is provided only via nc_client.h header file and only when libnetconf2 is compiled with libssh support.
 *
 * @param[in] pub_key Path to the public key.
 * @param[in] priv_key Path to the private key.
 *
 * @return EXIT_SUCCESS on success, EXIT_FAILURE otherwise.
 */
int nc_ssh_client_add_keypair(const char *pub_key, const char *priv_key);

/**
 * @brief Remove an SSH public and private key pair that was used for client authentication.
 *
 * Function is provided only via nc_client.h header file and only when libnetconf2 is compiled with libssh support.
 *
 * @param[in] idx Index of the keypair starting with 0.
 *
 * @return EXIT_SUCCESS on success, EXIT_FAILURE otherwise.
 */
int nc_ssh_client_del_keypair(int idx);

/**
 * @brief Get the number of public an private key pairs set to be used for client authentication.
 *
 * @return Set keypair count.
 */
int nc_ssh_client_get_keypair_count(void);

/**
 * @brief Get a specific keypair set to be used for client authentication.
 *
 * @param[in] idx Index of the specific keypair.
 * @param[out] pub_key Path to the public key.
 * @param[out] priv_key Path to the private key.
 *
 * @return EXIT_SUCCESS on success, EXIT_FAILURE otherwise.
 */
int nc_ssh_client_get_keypair(int idx, const char **pub_key, const char **priv_key);

/**
 * @brief Set SSH authentication method preference.
 *
 * Function is provided only via nc_client.h header file and only when libnetconf2 is compiled with libssh support.
 *
 * @param[in] auth_type Authentication method to modify the prefrence of.
 * @param[in] pref Preference of \p auth_type. Negative values disable the method.
 */
void nc_ssh_client_set_auth_pref(NC_SSH_AUTH_TYPE auth_type, short int pref);

/**
 * @brief Get SSH authentication method preference.
 *
 * Function is provided only via nc_client.h header file and only when libnetconf2 is compiled with libssh support.
 *
 * @param[in] auth_type Authentication method to retrieve the prefrence of.
 *
 * @return Preference of the \p auth_type.
 */
short int nc_ssh_client_get_auth_pref(NC_SSH_AUTH_TYPE auth_type);

#endif /* ENABLE_SSH */

#ifdef ENABLE_TLS

/**
 * @brief Initialize libssl context with certificates.
 *
 * Must be called before calling nc_connect_tls() or nc_connect_libssl()! This also initializes libssl, if your application
 * uses it, do not initialize it again.
 *
 * Function is provided only via nc_client.h header file and only when libnetconf2 is compiled with TLS support.
 *
 * @param[in] client_cert Path to the file containing the client certificate. If NULL, only initializes libssl/libcrypto.
 * @param[in] client_key Path to the file containing the private key for the \p client_cert.
 *                       If NULL, key is expected to be stored with \p client_cert.
 * @param[in] ca_file Location of the CA certificate file used to verify server certificates.
 *                    For more info, see the documentation for SSL_CTX_load_verify_locations() from OpenSSL.
 * @param[in] ca_dir Location of the CA certificates directory used to verify the server certificates.
 *                   For more info, see the documentation for SSL_CTX_load_verify_locations() from OpenSSL.
 * @param[in] crl_file Location of the CRL certificate file used to check for revocated certificates.
 * @param[in] crl_dir Location of the CRL certificate directory used to check for revocated certificates.
 *
 * @return EXIT_SUCCESS on success, EXIT_FAILURE otherwise.
 */
int nc_tls_client_init(const char *client_cert, const char *client_key, const char *ca_file, const char *ca_dir,
                       const char *crl_file, const char *crl_dir);

/**
 * @brief Destroy any dynamically allocated TLS-specific and libssl/librypto context.
 *
 * nc_connect_tls(), nc_connect_libssl() or any libssl/librypto functions cannnot be called after this.
 *
 * Function is provided only via nc_client.h header file and only when libnetconf2 is compiled with TLS support.
 */
void nc_tls_client_destroy(void);

/**
 * @brief Connect to the NETCONF server using TLS transport (via libssl)
 *
 * TLS session is created with the certificates set using nc_client_init_tls(), which must be called beforehand!
 * If the caller needs to use specific TLS session properties, they are supposed to use nc_connect_libssl().
 *
 * Function is provided only via nc_client.h header file and only when libnetconf2 is compiled with TLS support.
 *
 * @param[in] host Hostname or address (both Ipv4 and IPv6 are accepted) of the target server.
 *                 'localhost' is used by default if NULL is specified.
 * @param[in] port Port number of the target server. Default value 6513 is used if 0 is specified.
 * @param[in] ctx Optional parameter. If set, provides strict YANG context for the session
 *                (ignoring what is actually supported by the server side). If not set,
 *                YANG context is created for the session using \<get-schema\> (if supported
 *                by the server side) or/and by searching for YANG schemas in the searchpath
 *                (see nc_schema_searchpath()). In every case except not providing context
 *                to connect to a server supporting \<get-schema\> it is possible that
 *                the session context will not include all the models supported by the server.
 * @return Created NETCONF session object or NULL on error.
 */
struct nc_session *nc_connect_tls(const char *host, uint16_t port, struct ly_ctx *ctx);

/**
 * @brief Connect to the NETCONF server using the provided TLS (libssl) session.
 *
 * The TLS session supplied is expected to be fully connected and authenticated!
 *
 * Function is provided only via nc_client.h header file and only when libnetconf2 is compiled with TLS support.
 *
 * @param[in] tls libssl structure representing the TLS session object.
 * @param[in] ctx Optional parameter. If set, provides strict YANG context for the session
 *                (ignoring what is actually supported by the server side). If not set,
 *                YANG context is created for the session using \<get-schema\> (if supported
 *                by the server side) or/and by searching for YANG schemas in the searchpath
 *                (see nc_schema_searchpath()). In every case except not providing context
 *                to connect to a server supporting \<get-schema\> it is possible that
 *                the session context will not include all the models supported by the server.
 * @return Created NETCONF session object or NULL on error.
 */
struct nc_session *nc_connect_libssl(SSL *tls, struct ly_ctx *ctx);

/**
 * @brief Accept a Call Home TLS connection from a NETCONF server.
 *
 * Function is provided only via nc_client.h header file and only when libnetconf2 is compiled with TLS support.
 *
 * @param[in] port Port the NETCONF client will listen on.
 * @param[in] timeout Timeout for reading in milliseconds. Use negative value for infinite
 *                    waiting and 0 for immediate return if data are not available on wire.
 * @param[in] ctx Optional parameter. If set, provides strict YANG context for the session
 *                (ignoring what is actually supported by the server side). If not set,
 *                YANG context is created for the session using \<get-schema\> (if supported
 *                by the server side) or/and by searching for YANG schemas in the searchpath
 *                (see nc_schema_searchpath()). In every case except not providing context
 *                to connect to a server supporting \<get-schema\> it is possible that
 *                the session context will not include all the models supported by the server.
 * @return Created NETCONF session object or NULL on error.
 */
struct nc_session *nc_callhome_accept_tls(uint16_t port, int32_t timeout, struct ly_ctx *ctx);

#endif /* ENABLE_TLS */

/**
 * @brief Receive NETCONF RPC reply.
 *
 * If a reply to \<get\> or \<get-config\> RPCs is received, the data are the whole configuration
 * parsed (usually results in more top-level nodes), not just a single 'data' anyxml node with
 * the configuration unparsed inside (which would strictly be according to the model).
 *
 * @param[in] session NETCONF session from which the function gets data. It must be the
 *            client side session object.
 * @param[in] rpc Original RPC this should be the reply to.
 * @param[in] msgid Expected message ID of the reply.
 * @param[in] timeout Timeout for reading in milliseconds. Use negative value for infinite
 *            waiting and 0 for immediate return if data are not available on wire.
 * @param[out] reply Resulting object of NETCONF RPC reply.
 * @return NC_MSG_REPLY for success, NC_MSG_WOULDBLOCK if timeout reached and NC_MSG_ERROR
 *         when reading has failed.
 */
NC_MSG_TYPE nc_recv_reply(struct nc_session *session, struct nc_rpc *rpc, uint64_t msgid, int32_t timeout,
                          struct nc_reply **reply);

/**
 * @brief Receive NETCONF Notification.
 *
 * @param[in] session NETCONF session from which the function gets data. It must be the
 *            client side session object.
 * @param[in] timeout Timeout for reading in milliseconds. Use negative value for infinite
 *            waiting and 0 for immediate return if data are not available on wire.
 * @param[out] notif Resulting object of NETCONF Notification.
 * @return NC_MSG_NOTIF for success, NC_MSG_WOULDBLOCK if timeout reached and NC_MSG_ERROR
 *         when reading has failed.
 */
NC_MSG_TYPE nc_recv_notif(struct nc_session* session, int32_t timeout, struct nc_notif **notif);

/**
 * @brief Send NETCONF RPC message via the session.
 *
 * @param[in] session NETCONF session where the RPC will be written.
 * @param[in] rpc NETCOFN RPC object to send via specified session. Object can be created by
 *            nc_rpc_lock(), nc_rpc_unlock() and nc_rpc_generic() functions.
 * @param[in] timeout Timeout for writing in milliseconds. Use negative value for infinite
 *            waiting and 0 for return if data cannot be sent immediately.
 * @param[out] msgid If RPC was successfully sent, this is it's message ID.
 * @return #NC_MSG_RPC on success, #NC_MSG_WOULDBLOCK in case of busy session
 *         and #NC_MSG_ERROR on error.
 */
NC_MSG_TYPE nc_send_rpc(struct nc_session *session, struct nc_rpc *rpc, int32_t timeout, uint64_t *msgid);

#endif /* NC_SESSION_CLIENT_H_ */