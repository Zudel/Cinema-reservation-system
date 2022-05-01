#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include<stdbool.h>
#include "defines.h"



static void add_ticket(MYSQL *conn){
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[6];
	int status;



	if(!setup_prepared_stmt(&prepared_stmt, "call lista_proiezioni(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize projections list statement\n", false);
	}
	
		// Prepare parameters
		memset(param, 0, sizeof(param));


	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = conf.username;
	param[0].buffer_length = strlen(conf.username);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for add ticket insertion\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve projections list\n", true);
	}

	// We have multiple result sets here!
	do {
		// Skip OUT variables (although they are not present in the procedure...)
		if(conn->server_status & SERVER_PS_OUT_PARAMS) {
			goto next;
		}

			dump_result_set(conn, prepared_stmt, "");
			
		

		// more results? -1 = no, >0 = error, 0 = yes (keep looking)
	    next:
		status = mysql_stmt_next_result(prepared_stmt);
		if (status > 0)
			finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);
	} while (status == 0);
		
	mysql_stmt_close(prepared_stmt);

	// Input for the registration routine
	char lettera_posto;
	char numero_posto[5];
	char proiezione[46];
	int numero_posto_int;
	float prezzo_float;
	int proiezione_int;
	int codice_prenotazione;
	
	printf("\n\n");

	// Get the required information
	printf("Projection ID: ");
	getInput(46,proiezione,false);


	//convert values
	proiezione_int = atoi(proiezione);

	if(!setup_prepared_stmt(&prepared_stmt, "call lista_posti_disponibili(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize add_ticket statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type =MYSQL_TYPE_LONG;
	param[0].buffer = &proiezione_int;
	param[0].buffer_length = sizeof(proiezione_int);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for add seat insertion\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while adding the seat.");
	}

	// We have multiple result sets here!
	do {
		// Skip OUT variables (although they are not present in the procedure...)
		if(conn->server_status & SERVER_PS_OUT_PARAMS) {
			goto next2;
		}

			dump_result_set(conn, prepared_stmt, "");
			
		

		// more results? -1 = no, >0 = error, 0 = yes (keep looking)
	    next2:
		status = mysql_stmt_next_result(prepared_stmt);
		if (status > 0)
			finish_with_stmt_error(conn, prepared_stmt, "Unexpected condition", true);
	} while (status == 0);
	

	mysql_stmt_close(prepared_stmt);

	

	// Get the required information
	printf("Seat letter: ");
	getInput(1, &lettera_posto,false);
	printf("Seat number: ");
	getInput(2, numero_posto,false);
	


	//convert values
	numero_posto_int = atoi(numero_posto);


	if(!setup_prepared_stmt(&prepared_stmt, "call aggiungi_biglietto(?,?,?,?,?,?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize add_ticket statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type =MYSQL_TYPE_STRING;
	param[0].buffer = &lettera_posto;
	param[0]. buffer_length = sizeof(char);

	param[1].buffer_type =MYSQL_TYPE_LONG;
	param[1].buffer = &numero_posto_int;
	param[1]. buffer_length = sizeof(numero_posto_int);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = conf.username;
	param[2].buffer_length = strlen(conf.username);

	param[3].buffer_type =MYSQL_TYPE_LONG;
	param[3].buffer = &proiezione_int;
	param[3].buffer_length = sizeof(proiezione_int);

	param[4].buffer_type =MYSQL_TYPE_FLOAT; //OUT
	param[4].buffer = &prezzo_float;
	param[4].buffer_length = sizeof(prezzo_float);

	param[5].buffer_type =MYSQL_TYPE_LONG; //OUT
	param[5].buffer = &codice_prenotazione;
	param[5].buffer_length = sizeof(codice_prenotazione);



	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for add ticket insertion\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while adding the ticket.");
	} else {
		printf("\tticket correctly added...\n");
	}

	// Get back the ticket's price
	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_FLOAT; // OUT
	param[0].buffer = &prezzo_float;
	param[0].buffer_length = sizeof(prezzo_float);

	param[1].buffer_type = MYSQL_TYPE_LONG; // OUT
	param[1].buffer = &codice_prenotazione;
	param[1].buffer_length = sizeof(codice_prenotazione);
	
	if(mysql_stmt_bind_result(prepared_stmt, param)) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve output parameter", true);
	}
	
	// Retrieve output parameter
	if(mysql_stmt_fetch(prepared_stmt)) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not buffer results", true);
	}

	printf("\tticket price:  %4.2f\n", prezzo_float);
	printf("\tid ticket: %d", codice_prenotazione);

	mysql_stmt_close(prepared_stmt);

	//start routine "add_credit_card"

	// Input for the registration routine
	char intestatario_carta[46];
	char codice_cvv[46];
	char numero_carta[46];
	char data_scadenza_carta[20];
	int codice_cvv_int;
	long long int numero_carta_long;


	printf("\n\ninserire i dati della carta di credito.");
	
	// Input for the registration routine
	printf("\nNumber of credit card: ");
	getInput(46,numero_carta,false);
	printf("\nCVV: ");
	getInput(46,codice_cvv,false);
	printf("\naccountholder: ");
	getInput(46,intestatario_carta, false);
	printf("expiration date: ");
	getInput(20,data_scadenza_carta,false);

	//convert values
	numero_carta_long =  strtoll (numero_carta, NULL, 10);
	codice_cvv_int = atoi(codice_cvv);

	if(!setup_prepared_stmt(&prepared_stmt, "call aggiungi_carta_di_credito(?,?,?,?,?,?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize add_ticket statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = conf.username;
	param[0].buffer_length = strlen(conf.username);

	param[1].buffer_type = MYSQL_TYPE_LONGLONG;
	param[1].buffer = &numero_carta_long;
	param[1].buffer_length = sizeof(numero_carta_long);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = intestatario_carta;
	param[2].buffer_length = strlen(intestatario_carta);

	param[3].buffer_type = MYSQL_TYPE_LONG;
	param[3].buffer = &codice_cvv_int;
	param[3].buffer_length = sizeof(codice_cvv_int);

	param[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[4].buffer = data_scadenza_carta;
	param[4].buffer_length = strlen(data_scadenza_carta);

	param[5].buffer_type = MYSQL_TYPE_LONG; 
	param[5].buffer = &codice_prenotazione;
	param[5].buffer_length = sizeof(codice_prenotazione);


	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for add credit card insertion\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while adding the credit card.");
	} else {
		printf("\tcredit card correctly added...\n");
	}

	printf("\t\t***The booking Id is: %d \t\t\tConserve this for enter to the Cinema, thank you***!", codice_prenotazione);

	mysql_stmt_close(prepared_stmt);



}

static void delete_booking(MYSQL *conn){
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[2];

	// Input for the registration routine
	char codice_prenotazione[46];
	int codice_prenotazione_int;

	// Get the required information
	printf("booking id: ");
	getInput(46,codice_prenotazione,false);

	//convert values
	codice_prenotazione_int = atoi(codice_prenotazione);

	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call annulla_prenotazione(?,?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize delete_booking statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &codice_prenotazione_int;
	param[0].buffer_length = sizeof(codice_prenotazione_int);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = conf.username;
	param[1].buffer_length = strlen(conf.username);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for delete booking\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while deleting the booking.");
	} else {
		printf("booking is deleted correctly...\n");
	}

	mysql_stmt_close(prepared_stmt);

}



void run_as_customer(MYSQL *conn)
{
	char options[3] = {'1','2','3'};
	char op;
	
	printf("Switching to customer role...\n");

	if(!parse_config("/home/roberto/Scrivania/db/cliente.json", &conf)) {
		fprintf(stderr, "Unable to load professor configuration\n");
		exit(EXIT_FAILURE);
	}

	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}

	while(true) {
		printf("\033[2J\033[H");
		printf("*** What should I do for you? ***\n\n");
		printf("1) buy_ticket\n");
		printf("2) delete_booking\n");
		printf("3) Quit\n");

		op = multiChoice("Select an option", options, 3);

		switch(op) {
			case '1':
				add_ticket(conn);
				break;
				
			case '2':
			 	delete_booking(conn);
				break;	
			case '3':
				return;
				
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}

		getchar();
	}
}