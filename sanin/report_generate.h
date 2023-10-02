#ifndef _REPORT_GENERATE_H_
#define _REPORT_GENERATE_H_

void handle_get_html(json_t *root, struct client *client);
void handle_get_pdf(json_t *root, struct client *client);

#endif //_REPORT_GENERATE_H_
