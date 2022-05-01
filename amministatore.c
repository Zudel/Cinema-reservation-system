#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "defines.h"

static void show_employee(MYSQL*conn){
	MYSQL_STMT *prepared_stmt;
	int status;
	MYSQL_BIND param[1];

	if(!setup_prepared_stmt(&prepared_stmt, "call lista_dipendenti(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize work shift statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = conf.username;
	param[0].buffer_length = strlen(conf.username);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for employee list\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while showing the employee.");
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
}


static void add_work_shift(MYSQL *conn){
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[6];

	show_employee(conn); //call the store procedure "lista_dipendenti"
	printf("\n\n");

	// Input for the registration routine

	int matricola_int;
	char matricola[46];
	char Orario_inizio[21];
	char Orario_fine[21];
	char Data[21];

 
 
 	printf("enter the matricola: ");
 	getInput(46, matricola, false);
 	printf("Enter the time in hh:mm:ss format : ");
 	getInput(21,Orario_inizio,false);
 	printf("Enter the end time in hh:mm:ss format : ");
 	getInput(21,Orario_fine,false);
 	printf("date (in this form: 'year-month-day'):");
 	getInput(21,Data,false);

 	//appropriate conversions
 	matricola_int = atoi(matricola);


 	if(!setup_prepared_stmt(&prepared_stmt, "call aggiungi_turno_lavoro(?,?,?,?,?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize work shift statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = conf.username;
	param[0].buffer_length = strlen(conf.username);


	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = Data;
	param[1].buffer_length = strlen(Data);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = Orario_inizio;
	param[2].buffer_length = strlen(Orario_inizio);

	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[3].buffer = Orario_fine;
	param[3].buffer_length = strlen(Orario_fine);

	param[4].buffer_type = MYSQL_TYPE_LONG;
	param[4].buffer = &matricola_int;
	param[4].buffer_length = sizeof(matricola_int);



	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for work shift insertion\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while adding the work shift.");
	} else {
		printf("Work shift correctly added...\n");
	}


	mysql_stmt_close(prepared_stmt);


}



static void employee_report(MYSQL *conn){
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[1];
	int status;

	

	
	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call report_dipendenti(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize employee reports statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = conf.username;
	param[0].buffer_length = strlen(conf.username);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for employee reports\n", true);
	}
	

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while executing the routine.");
	}

	puts("\n\nnomi dei Cinema che in qualche fascia oraria sono sprovvisti di almeno due maschere e le informazioni delle proiezioni sprovvisti di proiezionista:");
	
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


}

static void bookings_report(MYSQL *conn) {
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[1];

 	if(!setup_prepared_stmt(&prepared_stmt, "call report_prenotazioni(?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize bookings report statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));
	
	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = conf.username;
	param[0].buffer_length = strlen(conf.username);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for bookings report\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while showing the bookings.");
	}

	// Dump the result set
	dump_result_set(conn, prepared_stmt, "\nList of bookings");

	mysql_stmt_close(prepared_stmt);

}


static void add_projection(MYSQL *conn){
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[6];

	// Input for the registration routine

	char data_proiezione[20];
	char ora_proiezione[20];
	char cinema[31];
	char film[46];
	char sala[2];
	int sala_int;
	int film_int;
	char proiezionista[46];
	bool is_null = true;

	// Get the required information
 	printf("\nCinema name: ");
 	getInput(31, cinema, false);
 	printf("hall number: ");
 	getInput(2,sala,false);
 	printf("film id: ");
 	getInput(46, film,false);
 	printf("film screening time: ");
 	getInput(20,ora_proiezione,false);
 	printf("film screening date (in this form: 'year month day'): ");
 	getInput(20,data_proiezione,false);

 	//appropriate conversions
 	sala_int = atoi(sala);
 	film_int = atoi(film);



 	if(!setup_prepared_stmt(&prepared_stmt, "call aggiungi_proiezione(?,?,?,?,?,?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize add projection  statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = &data_proiezione;
	param[0].buffer_length = strlen(data_proiezione);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = &ora_proiezione;
	param[1].buffer_length = strlen(ora_proiezione);

	param[2].buffer_type = MYSQL_TYPE_LONG;
	param[2].buffer = &film_int;
	param[2].buffer_length = sizeof(film_int);


	param[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[3].buffer = cinema;
	param[3].buffer_length = strlen(cinema);

	param[4].buffer_type = MYSQL_TYPE_LONG;
	param[4].buffer = &sala_int;
	param[4].buffer_length = sizeof(sala_int);

	param[5].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[5].buffer = proiezionista;
	param[5].buffer_length = strlen(proiezionista);
	param[5].is_null = &is_null;


if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for projection insertion\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while adding the projection.");
	} else {
		printf(" Projection correctly added...\n");
	}

	mysql_stmt_close(prepared_stmt);

}

static void create_user(MYSQL *conn){
	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[3];
	char options[3] = {'1','2', '3'};
	char r;

	// Input for the registration routine
	char username[46];
	char password[46];
	char ruolo[46];

	// Get the required information
	printf("\nUsername: ");
	getInput(46, username, false);
	printf("password: ");
	getInput(46, password, true);
	printf("Assign a possible role:\n");
	printf("\t1) Cliente\n");
	printf("\t2) Maschera\n");
	printf("\t3) Amministratore\n");
	r = multiChoice("Select role", options, 3);

	// Convert role into enum value
	switch(r) {
		case '1':
			strcpy(ruolo, "cliente");
			break;
		case '2':
			strcpy(ruolo, "maschera");
			break;
		case '3':
			strcpy(ruolo, "amministratore");
			break;
		default:
			fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
			abort();
	}

	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call crea_utente(?, ?, ?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize user insertion statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = username;
	param[0].buffer_length = strlen(username);

	param[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[1].buffer = password;
	param[1].buffer_length = strlen(password);

	param[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[2].buffer = ruolo;
	param[2].buffer_length = strlen(ruolo);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for user insertion\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error (prepared_stmt, "An error occurred while adding the user.");
	} else {
		printf("User correctly added...\n");
	}

	mysql_stmt_close(prepared_stmt);

}

static void add_customer(MYSQL *conn){

	MYSQL_STMT *prepared_stmt;
	MYSQL_BIND param[2];

	// Input for the registration routine
	char username[46];
	int id;

	// Get the required information
	printf("customer username: ");
	getInput(46, username, false);

	// Prepare stored procedure call
	if(!setup_prepared_stmt(&prepared_stmt, "call aggiungi_cliente(?, ?)", conn)) {
		finish_with_stmt_error(conn, prepared_stmt, "Unable to initialize customer insertion statement\n", false);
	}

	// Prepare parameters
	memset(param, 0, sizeof(param));

	param[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	param[0].buffer = username;
	param[0].buffer_length = strlen(username);

	param[1].buffer_type = MYSQL_TYPE_LONG; // OUT
	param[1].buffer = &id;
	param[1].buffer_length = sizeof(id);

	if (mysql_stmt_bind_param(prepared_stmt, param) != 0) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not bind parameters for customer insertion\n", true);
	}

	// Run procedure
	if (mysql_stmt_execute(prepared_stmt) != 0) {
		print_stmt_error(prepared_stmt, "An error occurred while adding customer.");
		goto out;
	}

	// Get back the ID of the newly-added customer
	memset(param, 0, sizeof(param));
	param[0].buffer_type = MYSQL_TYPE_LONG; // OUT
	param[0].buffer = &id;
	param[0].buffer_length = sizeof(id);
	
	if(mysql_stmt_bind_result(prepared_stmt, param)) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not retrieve output parameter", true);
	}
	
	// Retrieve output parameter
	if(mysql_stmt_fetch(prepared_stmt)) {
		finish_with_stmt_error(conn, prepared_stmt, "Could not buffer results", true);
	}

	printf("customer correctly added with ID %d...\n", id);

    out:
	mysql_stmt_close(prepared_stmt);

}

	void run_as_administrator(MYSQL *conn)
{
	char options[7] = {'1','2', '3', '4', '5','6','7'};
	char op;
	
	printf("Switching to administrative role...\n");

	if(!parse_config("/home/roberto/Scrivania/db/amministratore.json", &conf)) {
		fprintf(stderr, "Unable to load administrator configuration\n");
		exit(EXIT_FAILURE);
	}

	if(mysql_change_user(conn, conf.db_username, conf.db_password, conf.database)) {
		fprintf(stderr, "mysql_change_user() failed\n");
		exit(EXIT_FAILURE);
	}

	while(true) {
		printf("\033[2J\033[H");
		printf("*** What should I do for you? ***\n\n");
		printf("1) add_work_shift\n");
		printf("2) employee_reports\n");
		printf("3) bookings_report\n");
		printf("4) add_projection\n");
		printf("5) create_user\n");
		printf("6) add_customer\n");
		printf("7) Quit\n");

		op = multiChoice("Select an option", options, 7);

		switch(op) {
			
			case '1':
				add_work_shift(conn);
				break;
			
			case '2':
				employee_report(conn);
				break;
			
			case '3':
				bookings_report(conn);
				break;
			
			case '4':
				add_projection(conn);
				break;
			
			case '5':
				create_user(conn);
				break;
			
			case '6':
				add_customer(conn);
				break;
			
			case '7':
				return;
			
			default:
				fprintf(stderr, "Invalid condition at %s:%d\n", __FILE__, __LINE__);
				abort();
		}

		getchar();
	}
}