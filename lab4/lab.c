#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

typedef enum {SUCCESS, NO_INPUT_FILE, NO_OUTPUT_FILE, MODE_UNDEFINED, MODE_UNSUPPORTED, INPUT_CANNOT_READ, OUTPUT_CANNOT_WRITE, FAILURE} PARSE_RESULT;

static int processBySymbol(FILE* input, FILE* output)
{
if(input == NULL || output == NULL) return -1;
int buf;
do
{
	buf = fgetc(input);
	if(feof(input)) break;
	
	//write zero instead of o
	if(buf == 'o' || buf == 'O')
	{
		fputc('0', output);
		continue;
	}
	//write 1 instead of i
	if(buf == 'i' || buf == 'I')
	{
		fputc('1', output);
		continue;
	}
	fputc(buf, output);
}
while(1);
return 0;
}

static PARSE_RESULT process(const char* input, const char* output, const char* mode)
{
FILE *in, *out;
in = fopen(input, "r");
if(in == NULL) return INPUT_CANNOT_READ;

out = fopen(output, "w");
if(out == NULL) return OUTPUT_CANNOT_WRITE;

if(strcmp(mode, "BY_SYMB") == 0)
{
	if(processBySymbol(in, out) != 0)
		return FAILURE;
}
else if(strcmp(mode, "BY_STR") == 0)
{
	if(processBySymbol(in, out) != 0)
		return FAILURE;
}

fclose(in);
fclose(out);
return SUCCESS;
}

static PARSE_RESULT parse(int argc, char* argv[])
{
const char* input, *output, *mode;
bool modeParameterExists = false;
for(int i = 1; i < argc; ++i)
{
	if(strcmp(argv[i], "-i") == 0)
	{
		input = argv[++i];
		continue;
	}
	if(strcmp(argv[i], "-o") == 0)
	{
		output = argv[++i];
		continue;
	}
	if(strcmp(argv[i++], "-m") == 0)
	{
		modeParameterExists = true;
	}
	if(modeParameterExists)
	{
		if((strcmp(argv[i], "BY_SYMB") == 0) || (strcmp(argv[i], "BY_STR") == 0))
			mode = argv[i];
	}	
}
if(input == NULL) 	 return NO_INPUT_FILE;
if(output == NULL) 	 return NO_OUTPUT_FILE;
if(!modeParameterExists) return MODE_UNDEFINED;
if(mode == NULL) 	 return MODE_UNSUPPORTED;

return process(input, output, mode);
}

int main(int argc, char* argv[])
{
if(argc < 2) return -1;
if(strcmp(argv[1], "-help") == 0)
{
	printf("Use \"lab -help\" to see this help\n");
	printf("Use \"lab -i <input file> -o <output file> -m <mode>\" to parse <input file> and write result to <output file>.\nParse modes are the following:\n\t- BY_SYMB to parse input file by symbols\n\t- BY_STR to parse input file by strings\n");
	return 0;
}
if(argc < 6)
{
	printf("You should define input and output files and parsing mode\n");
	printf("Type lab -help to see help information\n");
	return -1;
}
PARSE_RESULT result = parse(argc, argv);
switch(result)
{
case SUCCESS:
	printf("File parsed successfully. See output file for results\n");
	break;
case NO_INPUT_FILE:
	printf("You should define input file with -i parameter. See -help for more information\n");
	break;
case NO_OUTPUT_FILE:
	printf("You should define output file with -o parameter. See -help for more information\n");
	break;
case MODE_UNDEFINED:
	printf("You should define parsing mode with -m parameter. See -help for more information\n");
	break;
case MODE_UNSUPPORTED:
	printf("Parsing mode defined with -m parameter is unsupported. See -help for more information\n");
	break;
case INPUT_CANNOT_READ:
	printf("Input file cannot be read. Please, check you have appropriate rights\n");
	break;
case OUTPUT_CANNOT_WRITE:
	printf("Output file cannot be written to. Please, check you have appropriate rights\n");
	break;
default:
	printf("----------\n");
	break;
}
return 0;
}

