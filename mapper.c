#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STEP 5

typedef struct{
  short int function_code;
  short int mes_type;
  short int mes_data[10];
  short int cnt_data;
}messageType_t;

messageType_t *message;
int cnt_mes;
int cnt_mes_total;
short int cnt_BLE;
short int cnt_UWB;

char *read_line(void)
{
  char *line = NULL;
  int ch = 0;
  int i = 0;
  int current_size = 0;
  
  while ((ch = getchar()) != EOF) 
    {
      if (i == current_size)
	{	  
	  current_size = current_size + STEP; 
	  line = realloc(line, current_size * sizeof(char));
	}
      if (ch != '\n')
	{
	  *(line + i++) = ch;
	}
      else
	{
	  break;
	}
    }

  if (line != NULL)
    {
      if (i == current_size) 
	{
	  current_size++;
	  line = realloc(line, current_size * sizeof(char));
	}
      *(line + i) = 0; 
    }

  return line;
}

void separate_line(char *s)
{ 
  char *sep;
  short int function_code=0;
  short int cnt_byte = 0;
  short int data[10];
  short int type = 0;
  short int error_check = 0;//daca e 1 mesajul e eronat si il sterg din sir
  
  for(int i=0; i<30; i++)
    printf("-");
  printf("\n");
  
  sep = strtok(s," ");
  
  while(sep)
    {
      int nr = strtol(sep, NULL, 16);

      if(!nr)
	{
	  fprintf(stderr, "only hex numbers please! Message number: %d\n", cnt_mes_total);
	  error_check = 1;
	}
      
      if(nr == 0x08 || nr == 0x09)
	{
	  cnt_mes_total++;//pentru a afisa locatia erorilor daca apar

	  //verific daca mesajul anterior este bun
	  
	  if(function_code == 0x04 && (cnt_byte > 4 || cnt_byte < 1)  && error_check==0)
	    {
	      if(cnt_byte == 0)
		{
		  fprintf(stderr, "too few bytes for owner pairing function code! Message number: %d\n", cnt_mes_total); 
		}
		
	      if(cnt_byte > 4)
		{
		  fprintf(stderr, "too many bytes for owner pairing function code! Message number: %d\n", cnt_mes_total); 
		}
	      error_check = 1;
	    }
	  else
	    if(function_code == 0x05 && (cnt_byte > 7 || cnt_byte < 1)  && error_check==0)
	      {
		if(cnt_byte == 0)
		  {
		    fprintf(stderr, "too few bytes for scan device function code! Message number: %d\n", cnt_mes_total); 
		  }
		
		if(cnt_byte > 4)
		  {
		    fprintf(stderr, "too many bytes for scan device function code! Message number: %d\n", cnt_mes_total); 
		  }
	        error_check = 1;
	      }
	    else
	      if((function_code == 0x06 || function_code == 0x09) && (cnt_byte != 2)   && error_check==0)
		{
		  if(cnt_byte < 2 && function_code == 0x06)
		    {
		      fprintf(stderr, "too few bytes for start session function code! Message number: %d\n", cnt_mes_total); 
		    }
		
		  if(cnt_byte > 2 && function_code == 0x06)
		    {
		      fprintf(stderr, "too many bytes for start ranging function code! Message number: %d\n", cnt_mes_total); 
		    }
		  if(cnt_byte < 2 && function_code == 0x07)
		    {
		      fprintf(stderr, "too few bytes for start ranging function code! Message number: %d\n", cnt_mes_total); 
		    }
		
		  if(cnt_byte > 2 && function_code == 0x07)
		    {
		      fprintf(stderr, "too many bytes for start session function code! Message number: %d\n", cnt_mes_total); 
		    }
		  error_check = 1;
		}

	  if((function_code == 0x04 || function_code == 0x05) && type == 0x09 && error_check==0)
	    {
	      if(function_code == 0x04)
		{
		  fprintf(stderr, "owner pairing is not an UWB message! Message number: %d\n", cnt_mes_total);
		}
	      else
		{
		  fprintf(stderr, "scan device is not an UWB message! Message number: %d\n", cnt_mes_total);
		}
	      error_check = 1;
	    }
	  else
	    if((function_code == 0x06 || function_code == 0x07) && type == 0x08  && error_check==0)
	      {
		if(function_code == 0x06)
		  {
		    fprintf(stderr, "start session is not a BLE message! Message number: %d\n", cnt_mes_total);
		  }
		else
		  {
		    fprintf(stderr, "start ranging is not a BLE message! Message number: %d\n", cnt_mes_total);
		  }
		error_check = 1;
	      }

	  if(function_code == 0 && cnt_mes>0 && error_check==0)
	    {
	      fprintf(stderr, "no function code for this message! Message number: %d\n", cnt_mes_total);
	      error_check = 1;
	    }
	  if(cnt_byte == 0 && cnt_mes>0 && error_check==0)
	    {
	      fprintf(stderr, "no data introduced for this message! Message number: %d\n", cnt_mes_total);
	      error_check = 1;
	    }

	  //daca nu are erori il adaug in structura
	  if(error_check == 0 && cnt_byte>0)
	    {
	       
	      message = realloc(message, (cnt_mes + 1) * sizeof(messageType_t));
	      if(!message)
		{
		  fprintf(stderr, "alloc error message nr %d!\n", cnt_mes_total);
		  exit(-1);
		}
	      message[cnt_mes].mes_type = type;
	      message[cnt_mes].function_code = function_code;
	      for(int i=0; i<cnt_byte; i++)
		{ 
		  message[cnt_mes].mes_data[i] = data[i];
		}
	      message[cnt_mes].cnt_data = cnt_byte;
	      cnt_mes++;
	    }

	  
	  type = nr;
	  error_check = 0;
	  cnt_byte = 0;
	  function_code = 0;
	}
      else
	if(nr == 0x04 || nr == 0x05 || nr == 0x06 || nr == 0x07)
	  function_code = nr;
	else
	  if(nr >= 0xAA && nr <= 0xFF)
	    {
	      data[cnt_byte] = nr;
	      cnt_byte++; 
	    }
	  else
	    if(nr > 0xFF)
	      {
		fprintf(stderr, "0x%.2X is too big of a number for this message! Message number: %d\n", nr, cnt_mes_total);
		error_check = 1;
	      }
	    else
	      if(nr < 0xAA)
		{
		  fprintf(stderr, "0x%.2X is too small of a number for this message! Message number: %d\n", nr, cnt_mes_total);
		  error_check = 1;
		}

      
	
      sep = strtok(NULL," ");
    }

  //verific si ultimul mesaj
  if(function_code == 0x04 && (cnt_byte > 4 || cnt_byte < 1) && error_check==0)
    {
      if(cnt_byte == 0)
	{
	  fprintf(stderr, "too few bytes for owner pairing function code! Message number: %d\n", cnt_mes_total); 
	}
		
      if(cnt_byte > 4)
	{
	  fprintf(stderr, "too many bytes for owner pairing function code! Message number: %d\n", cnt_mes_total); 
	}
      error_check = 1;
    }
  else
    if(function_code == 0x05 && (cnt_byte > 7 || cnt_byte < 1) && error_check==0)
      {
	if(cnt_byte == 0)
	  {
	    fprintf(stderr, "too few bytes for scan device function code! Message number: %d\n", cnt_mes_total); 
	  }
		
	if(cnt_byte > 4)
	  {
	    fprintf(stderr, "too many bytes for scan device function code! Message number: %d\n", cnt_mes_total); 
	  }
        error_check = 1;
      }
    else
      if((function_code == 0x06 || function_code == 0x07) && (cnt_byte != 2) && error_check==0)
	{
	  if(cnt_byte < 2 && function_code == 0x06)
	    {
	      fprintf(stderr, "too few bytes for start session function code! Message number: %d\n", cnt_mes_total); 
	    }
		
	  if(cnt_byte > 2 && function_code == 0x06)
	    {
	      fprintf(stderr, "too many bytes for start ranging function code! Message number: %d\n", cnt_mes_total); 
	    }
	  
	  if(cnt_byte < 2 && function_code == 0x07)
	    {
	      fprintf(stderr, "too few bytes for start ranging function code! Message number: %d\n", cnt_mes_total); 
	    }
		
	  if(cnt_byte > 2 && function_code == 0x07)
	    {
	      fprintf(stderr, "too many bytes for start session function code! Message number: %d\n", cnt_mes_total); 
	    }
	  error_check = 1;
	}

  if((function_code == 0x04 || function_code == 0x05) && type != 0x08 && error_check==0)
    {
      if(function_code == 0x04)
	{
	  fprintf(stderr, "owner pairing is not an UWB message! Message number: %d\n", cnt_mes_total);
	}
      else
	{
	  fprintf(stderr, "scan device is not an UWB message! Message number: %d\n", cnt_mes_total);
	}
      error_check = 1;
    }
  else
    if((function_code == 0x06 || function_code == 0x07) && type != 0x09  && error_check==0)
      {
	if(function_code == 0x06)
	  {
	    fprintf(stderr, "start session is not a BLE message! Message number: %d\n", cnt_mes_total);
	  }
	else
	  {
	    fprintf(stderr, "start ranging is not a BLE message! Message number: %d\n", cnt_mes_total);
	  }
	error_check = 1;
      }
  
  if(function_code == 0 && error_check==0)
    {
      fprintf(stderr, "no function code for for the message number: %d\n", cnt_mes_total);
      error_check = 1;
    }
  if(cnt_byte == 0 && error_check==0)
    {
      fprintf(stderr, "no data introduced for the message number: %d\n", cnt_mes_total);
      error_check = 1;
    }

  if(error_check == 0)
    {
      message = realloc(message, (cnt_mes + 1) * sizeof(messageType_t));
      if(!message)
	{
	  fprintf(stderr, "alloc error message nr %d!\n", cnt_mes);
	  exit(-1);
	}
      message[cnt_mes].mes_type = type;
      message[cnt_mes].function_code = function_code;
      for(int i=0; i<cnt_byte; i++)
	{ 
	  message[cnt_mes].mes_data[i] = data[i];
	}
      message[cnt_mes].cnt_data = cnt_byte;
      cnt_mes++;
    }
			
		
}


void print_message(void)
{
  //pentru aspect placut nimic altceva
  if(cnt_mes != cnt_mes_total)
    {
      for(int i=0; i<30; i++)
    	printf("-");
      printf("\n");
    }
  
  for(int i=0; i < cnt_mes; i++)
    {
      if(message[i].mes_type == 0x08)
	{
	  cnt_BLE++;
	}
      else if(message[i].mes_type == 0x09)
	{
	  cnt_UWB++;
	}
    }
  if(cnt_BLE + cnt_UWB > 0)
    {
      printf("Number of BLE messages: %d\nNumber of UWB messages: %d\n",cnt_BLE, cnt_UWB);
    }

  
  for(int i=0; i < cnt_mes; i++)
    {
      printf("Message %d:\n", i+1);
      switch(message[i].mes_type)
	{
	case 0x08:
	  printf("Message type: BLE (0x08)\n");
	  break;
	case 0x09:
	  printf("Message type: UWB (0x09)\n");
	  break;
	}

      switch(message[i].function_code)
	{
	case 0x04:
	  printf("Function code: Owner Pairing(0x04)\n");
	  break;
	case 0x05:
	  printf("Function code: Scan Device(0x05)\n");
	  break;
	case 0x06:
	  printf("Function code: Start Session(0x06)\n");
	  break;
	case 0x07:
	  printf("Function code: Start Ranging(0x07)\n");
	  break;
	}
	
      switch(message[i].function_code)
	{
	case 0x04:
	  printf("Data: Owner Key ID");
	  break;
	case 0x05:
	  printf("Data: Device ID");
	  break;
	case 0x06:
	  printf("Data: Time of Flight");
	  break;
	case 0x07:
	  printf("Data: Time of Flight");
	  break;
	}
      printf("(");
 
      for(int j=0; j < message[i].cnt_data - 1; j++)
	{
	  printf("0x%.2X ", message[i].mes_data[j]);
	}
      printf("0x%.2X)\n", message[i].mes_data[message[i].cnt_data - 1]);
    }
}

int main()
{
  char *message_str = NULL;
  message_str = read_line(); //citesc dinamic sirul primit
  
  separate_line(message_str); //adaug in structura mesajele corecte
  print_message(); //afisez mesajele

  //never memory leaks
  free(message_str);
  free(message);
  
  return 0;
}
