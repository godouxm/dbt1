/*
 * tm_interface.c
 *
 * This file is released under the terms of the Artistic License.  Please see
 * the file LICENSE, included in this package, for details.
 *
 * Copyright (C) 2002 Mark Wong & Jenny Zhang &
 *                    Open Source Development Lab, Inc.
 *
 * 18 march 2002
 */

#include <common.h>
#include <tm_interface.h>
#include <_socket.h>
#include <eu.h>

int receive_interaction_packet(int s, struct eu_context_t *euc)
{
	void *data = NULL;
	int length = 0;
	int rec;
	int result;

	/* Receive transaction type. */
	if ((rec = _receive(s, (void *) &euc->interaction, sizeof(euc->interaction))) == -1)
	{
		LOG_ERROR_MESSAGE("cannot receive interaction type");
		return ERROR;
	}
	if (rec == 0) return SOCKET_CLOSE;
	
	/* receive if the transaction succeeds */
	if ((rec = _receive(s, (void *) &result, sizeof(int))) == -1)
	{
		LOG_ERROR_MESSAGE("cannot receive result");
		return ERROR;
	}
	if (rec == 0) return SOCKET_CLOSE;
	
	if (result == ERROR) 
	{
		LOG_ERROR_MESSAGE("database transaction failed");
		return ERROR;
	}

	/* Receive transaction data. */
	switch (euc->interaction)
	{
		case ADMIN_CONFIRM:
			data = &euc->admin_confirm_data;
			length = sizeof(struct admin_confirm_t);
			break;
		case ADMIN_REQUEST:
			data = &euc->admin_request_data;
			length = sizeof(struct admin_request_t);
			break;
		case BEST_SELLERS:
			data = &euc->best_sellers_data;
			length = sizeof(struct best_sellers_t);
			break;
		case BUY_CONFIRM:
			data = &euc->buy_confirm_data;
			length = sizeof(struct buy_confirm_t);
			break;
		case BUY_REQUEST:
			data = &euc->buy_request_data;
			length = sizeof(struct buy_request_t);
			break;
		case HOME:
			data = &euc->home_data;
			length = sizeof(struct home_t);
			break;
		case NEW_PRODUCTS:
			data = &euc->new_products_data;
			length = sizeof(struct new_products_t);
			break;
		case ORDER_DISPLAY:
			data = &euc->order_display_data;
			length = sizeof(struct order_display_t);
			break;
		case ORDER_INQUIRY:
			data = &euc->order_inquiry_data;
			length = sizeof(struct order_inquiry_t);
			break;
		case PRODUCT_DETAIL:
			data = &euc->product_detail_data;
			length = sizeof(struct product_detail_t);
			break;
		case SEARCH_REQUEST:
			data = &euc->search_request_data;
			length = sizeof(struct search_request_t);
			break;
		case SEARCH_RESULTS:
			data = &euc->search_results_data;
			length = sizeof(struct search_results_t);
			break;
		case SHOPPING_CART:
			data = &euc->shopping_cart_data;
			length = sizeof(struct shopping_cart_t);
			break;
	}
	if ((rec = _receive(s, data, length)) == -1)
	{
		LOG_ERROR_MESSAGE("cannot receive interaction data");
		return ERROR;
	}
	if (rec == 0) return SOCKET_CLOSE;

	return OK;
}

int send_interaction_packet(int s, struct eu_context_t *euc)
{
	void *data = NULL;
	int length = 0;

	switch (euc->interaction)
	{
		case ADMIN_CONFIRM:
			data = &euc->admin_confirm_data;
			length = sizeof(struct admin_confirm_t);
			break;
		case ADMIN_REQUEST:
			data = &euc->admin_request_data;
			length = sizeof(struct admin_request_t);
			break;
		case BEST_SELLERS:
			data = &euc->best_sellers_data;
			length = sizeof(struct best_sellers_t);
			break;
		case BUY_CONFIRM:
			data = &euc->buy_confirm_data;
			length = sizeof(struct buy_confirm_t);
			break;
		case BUY_REQUEST:
			data = &euc->buy_request_data;
			length = sizeof(struct buy_request_t);
			break;
		case HOME:
			data = &euc->home_data;
			length = sizeof(struct home_t);
			break;
		case NEW_PRODUCTS:
			data = &euc->new_products_data;
			length = sizeof(struct new_products_t);
			break;
		case ORDER_DISPLAY:
			data = &euc->order_display_data;
			length = sizeof(struct order_display_t);
			break;
		case ORDER_INQUIRY:
			data = &euc->order_inquiry_data;
			length = sizeof(struct order_inquiry_t);
			break;
		case PRODUCT_DETAIL:
			data = &euc->product_detail_data;
			length = sizeof(struct product_detail_t);
			break;
		case SEARCH_REQUEST:
			data = &euc->search_request_data;
			length = sizeof(struct search_request_t);
			break;
		case SEARCH_RESULTS:
			data = &euc->search_results_data;
			length = sizeof(struct search_results_t);
			break;
		case SHOPPING_CART:
			data = &euc->shopping_cart_data;
			length = sizeof(struct shopping_cart_t);
			break;
	}

	/* Send transaction type. */
	if (_send(s, (void *) &euc->interaction, sizeof(euc->interaction)) == -1)
	{
		LOG_ERROR_MESSAGE("cannot send type %s", euc->interaction);
		return ERROR;
	}

	/* Send transaction data. */
	if (_send(s, data, length) == -1)
	{
		LOG_ERROR_MESSAGE("cannot send data %s", euc->interaction);
		return ERROR;
	}
	return OK;
}
