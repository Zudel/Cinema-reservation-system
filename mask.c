#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdbool.h>

#include "defines.h"


static void print_ticket(int prenotazione, MYSQL *conn){
		MYSQL_BIND param[2];
		MYSQL_STMT *prepared_stmt;


	 // Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call stampa_biglietto(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize code insertion statement\n", false);
	}	

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_LONG;
	param[0].buffer = &prenotazione;
	param[0].buffer_length = sizeof(prenotazione);

	//bind the parameters
	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for code insertion\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while printing the ticket.");
	} 
	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nticket");

	mysql_stmt_close(prepared_stmt);
}


static void booking_validation(MYSQL *conn){
	MYSQL_BIND param[2];
	MYSQL_STMT *prepared_stmt;
	bool esito;

	// Input for the registration routine
	char codice_prenotazione[46];
	int codice_prenotazione_int;

	// Get the required information
	printf("Booking code:");
	getInput(46,codice_prenotazione,false);



	// Apply proper type conversions
	codice_prenotazione_int = atoi(codice_prenotazione);

	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call convalida_biglietto(?, ?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize code insertion statement\n", false);
	}	

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = conf.username;
	param[0].buffer_length = strlen(conf.username);

	param[1].buffer_type =MYSQL_TYPE_LONG;
	param[1].buffer = &codice_prenotazione_int;
	param[1].buffer_length = sizeof(codice_prenotazione_int);

	//bind the parameters
	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for code insertion\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while validated the booking code.");
	} 
	else {
		printf("booking code correctly validated...\n");
	}

	mysql_stmt_close(prepared_stmt);

	esito = yesOrNo("Vuoi stampare il biglietto convalidato?", 's', 'n', false, true);
	if(esito)
		print_ticket(codice_prenotazione_int,conn);
	else
		return;
}



void run_as_mask(MYSQL *conn){
	char options[3] = {'1','2'};
	char op;

	printf("Switching to mask role...\n");

	if(!parse_config("/home/roberto/Scrivania/db/maschera.json", &conf)) {
		fprintf(stderr, "Unable to load mask configuration\n");
		exit(EXIT_FAILURE);
	}

	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}
	while(true){
		printf("\033[2J\033[H");
		printf("*** What should I do for you? ***\n\n");
		printf("1) convalidate booking code\n");
		printf("2) Quit\n");

		op = multiChoice("Select an option", options, 2);

		switch(op){
			case '1':
				booking_validation(conn);
				break;
			case '2':
				return;


			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();



		}

		getchar();
	}

}