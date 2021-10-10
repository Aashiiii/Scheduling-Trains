#include <stdio.h>

#define buffer_size 4098
#define max_hour 23
#define max_min_sec 59
#define ascii_0 48
#define ascii_9 57
#define ascii_A 65
#define ascii_Z 90

/* The check_format function checks the hh:mm:ss values. hh can lie
 between 0 and 23, min and sec can lie between 0 and 59.
Arguments -
time - array of int with format {hour, min, sec}
Return value -
0 -> format is correct according to 24 hour format
-1 -> wrong format
*/

int check_format (int* time) {

	if (time[0] < 0 || time[0] > max_hour || 
		time[1] < 0 || time[1] > max_min_sec || 
		time[2] < 0 || time[2] > max_min_sec ) {
			return -1;
	}
	
	return 0;
}

/* The time_compare function checks whether time1 comes after time2 or not.
Arguments -
time1 - array of int with format {hour, min, sec}
time2 - array of int with format {hour, min, sec}
Return value -
0 -> time1 comes after time2
-1 -> time1 comes before time2 (time2 occurs first)
*/

int time_compare (int* time1, int* time2 ){

	if ((time1[0] > time2[0]) || 
		(time1[0] == time2[0] && time1[1] > time2[1] ) || 
		(time1[0] == time2[0] && time1[1] == time2[1] && 
		time1[2] >= time2[2])) {
			return 0;
		}
		
	return -1;
}

/* The check_time function checks whether time string is in correct format
 or not, that is hh:mm:ss
Arguments -
time - c string time
Return value -
0 -> time is in correct format
-1 -> time is not in correct format
*/

int check_time (char *time) {

	int pos = 0;
	for (pos = 0; pos < 8; pos++) {
		if((pos + 1) % 3 == 0) {
			if(time[pos]!=':') {
				/* Colons not at correct position.*/
				return -1;
			}
		}
		else {
			if (time[pos] < ascii_0 || time[pos] > ascii_9) {
			/*Characters in string are not digits (0-9) */
				return -1;
			}
			}
	}
	
	return 0;
}

/* The function to_lower checks if a character is in upper case, if so 
it changes it to lower case. 
We use this function to compare source, and destibation names correctly
even if they are in different case.
Arguments-
ch - the input character
Return value-
ch - lower case equivalent of character passed as argument (if character
 passed was in upper case A-Z)
*/

char to_lower ( char ch ) {
	if (ch >= ascii_A && ch <= ascii_A) {
	/* Character is an uppercase alphabet (A-Z) */
		ch += 32;
	}
	return ch;
}

/* Main method
Arguments-
argc - number of command line arguments
argv -  array of address of command line arguments
Return value-
0 -> If next train found.
1 -> If no suitable train found, or in case of wrong command line arguments.
*/
int main (int argc, char **argv) {

	/* Checking number of arguments.*/
	if (argc <= 3) {
		printf("Please provide <source> <destination> <time> as command line arguments\n");
		return 1;
	}
	if(argc > 4) {
		printf("Too many arguments. Please provide <source> <destination> <time> as command line arguments\n");
		return 1;
	}
	if (check_time (argv[3]) == -1) {
		printf("Invalid time argument\n");
		return 1;
	}
	
	
	/* buffer_size is 4098
	Entry (4096)  + new line character + null added by fgets */
	char line [buffer_size];
	
	/* sameday_time stores the earliest time at which train is
	 available after the arrival time on station */
	int sameday_time[3] = { max_hour, max_min_sec, max_min_sec };
	 
	
	/* nextday_time stores the earliest train available from specified
	 source to destination on all days. */
	int nextday_time[3] = { max_hour, max_min_sec, max_min_sec };
	
	int same = -1;
	int next = -1;
	
	int num_of_lines = 0;
	
	/* Looping through all the entries in the input file*/
	while ( fgets (line, sizeof(line), stdin) != NULL) {
	
		/* Finds the number of lines in the input file. */
		num_of_lines += 1;
	
		int len = 0;
	
		/* We will update possible to -1 if the entry source and
		 destination would not match, or if the entry would have
		  wrong format, or if time format would be wrong.*/
		int possible = 0;
	
		/* Finding the length of each timetable entry*/
		for (len=0; line [len] != '\0'; ++len);
		
		len -= 1;
		
		char *point = line;
		int colon = 0;
		int i = 0;
		char each_char;
		
		while (*point != '\0') {
			point++;
			i++;
			each_char = line [i];
	
			/* Counting the number of colons in the entry
			 line.*/
			if (each_char == ':') {
				colon++;
			}
		}

		if(colon < 6){
			continue;
		}
	
		i = -1;
	
		/* Checking if the source given by user matches the
		 timetable entry. We set possible to -1 when it does not
		  matches.  */
		char *sourcep = argv[1];
		while (*sourcep != '\0'){
			sourcep++;
			i++;
			if (to_lower (line[i]) != to_lower (argv [1][i]) ) {
				possible = -1;
			}
		}
	
		/* Checking if source is followed by two colons in the
		 entry or not. We set possible to -1, if it is not the
		  case*/
		if (line[i+1] != ':' || line[i+2] != ':') {
			possible = -1;
		} 
		i = i + 2;
		int j = 0;
	
		/* Checking if the destination given by user matches the
		 timetable entry. We set possible to -1 when it does not
		  matches.  */
		char *destp = argv[2];
		while (*destp != '\0') {
			destp++;
			i++;
			if(to_lower(line[i]) != to_lower(argv[2][j])){
				possible = -1;
			}
			j+=1;
		}
	
		/* Checking if source is followed by two colons in the
		 entry or not. We set possible to -1, if it is not the
		  case*/
		if (line[i+1] != ':' || line[i+2] != ':') {
			possible = -1;
		} 
		i=i+2;
	
		/*Checking the time*/
		char *poi = &line [i+1];
		if (check_time (poi) == -1 ){
			possible = -1;
		}
	
		/* Finding hour, minute, and second values of the time in
		 the timetable entry*/
		int hour = ((line[i+1] - ascii_0) * 10 ) + line[i+2] - ascii_0 ;
		int min = ((line[i+4] - ascii_0)*10 ) + line[i+5] - ascii_0;
		int sec = ((line[i+7] - ascii_0)*10 ) + line[i+8] - ascii_0;
	
		int timetable_time[3] = { hour, min, sec };
	
		/* Finding hour, minute, and second values of the time
		 passed as argument*/
		hour = ((argv[3][0] - ascii_0)*10) + argv[3][1] - ascii_0;
		min = ((argv[3][3] - ascii_0)*10) + argv[3][4] - ascii_0;
		sec = ((argv[3][6] - ascii_0)*10) + argv[3][7] - ascii_0;
	
		int arg_time[3] = { hour, min, sec };
		
		/* Checking that there are no more characters in timetable
		 entry. If yes, then the entry is invalid, as the entry
		  needs to have new line character following the time*/
		if ( line[i + 9] != '\n' ) {
			possible = -1;
		}
	
		/* Checking if the time in timetable entry corresponds to
		 24 hour format correctly or not. We set possible to -1 if
		  it is not the case*/
		if (check_format (timetable_time) == -1 ) {
			possible = -1;
		}
	
		/* Checking if the time passed as command line argument
		 corresponds to 24 hour format correctly or not. If not we
		  exit the code with appropriate message. If the time user
		   has passed is incorrect, then we do not have to check
		    timetable entries.*/
		if( check_format (arg_time) == -1 ){
			printf ("Invalid time argument\n");
			return 1;
		}
	
		/* If possible is equal to -1 then we skip this entry.*/
		if(possible == -1) {
			continue;
		}
	
		/* We enter this part of code only if the source and destination match.*/
	
		/* If the time in timetable entry is before the time the
		 user arrived at station, then we try to find the earliest
		  train available next day.*/
		if (time_compare (arg_time, timetable_time) == 0) {
	
			if (time_compare (nextday_time, timetable_time) == 0) {
	 			nextday_time[0] = timetable_time[0];
				nextday_time[1] = timetable_time[1];
	 			nextday_time[2] = timetable_time[2];
	 			next = 0;
			}
	
			continue;
		}
		/* We enter this part only if the time in timetable entry
		 is after the user's arrival time on station. In this case
		  we find the earliest train after the time of user's
		   arrival on station*/
	
		if (time_compare (sameday_time, timetable_time) == 0) {
	 		sameday_time[0] = timetable_time[0];
	 		sameday_time[1] = timetable_time[1];
	 		sameday_time[2] = timetable_time[2];
	 		same = 0;
		}
	
	
	}
	int exit_status = 1;
	
	/* If number of lines input is zero, then that means no timetable
	 was passed*/
	if (num_of_lines == 0) {
		printf("No timetable to process\n");
	}
	
	/* We check if we have any train available same day*/
	else if (same == 0) {
		exit_status = 0;
		printf("The next train to %s from %s departs at %02d:%02d:%02d\n",
		argv[2],argv[1],sameday_time[0],sameday_time[1],sameday_time[2] );
	}

	/* If we dont have any train available the same day, then we print
	 the earliest train available next day*/
	else if (next == 0 ) {
		exit_status = 0;
		printf("The next train to %s from %s departs at %02d:%02d:%02d\n",
		argv[2],argv[1],nextday_time[0],nextday_time[1],nextday_time[2]);
	}
	
	/* No trains found.*/
	else {
		printf("No suitable trains can be found\n");
	}
return exit_status;	
}

















