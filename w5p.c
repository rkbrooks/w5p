// Takes a text file, line or nothing-delimited list of words and packs them into a 5-bit-per-letter
// binary file.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

void putBit(char *buf, unsigned short bit, unsigned long bitPointer) {
	
	unsigned int bytePointer;

	// set or clear the appropriate bit position

	bytePointer = bitPointer>>3;

	#ifdef DEBUG
		printf("Called putBit with %i, bitPointer %lu and bytePointer %u\n",bit,bitPointer,bytePointer);
		printf("%hu",bit);   // note that this will print in little-bitwise-endian (aka R to L reading bits) the way it is called in this proggie
	#endif

	if(bit) {
		buf[bytePointer] |= 1 << (7-(bitPointer & 7));
	} else {
      	buf[bytePointer] &= ~(1 << (7-(bitPointer & 7)));
	}

}

int main(int argc, char *argv[])
{

	char *inbuf = NULL;	// input and normalization buffer
 	char *outbuf = NULL;	// output buffer

	char  current;

	FILE *fh;
	long inputFileSize;
	int i,j; // loop counters

	unsigned long bitPointer=0;   


	if(argc != 3) {
		printf("Error: Use program with \"%s <inputfile> <outputfile>\"\n",argv[0]);
		return(1);
	}
	
	fh = fopen(argv[1],"rb");

	if(!fh) {
		printf("Error: Unable to open input file\n");
		return(1);
	}

	if(fseek(fh, 0L, SEEK_END)) {
		printf("Error: Unable to seek to end of file\n");
		return(1);
	}

	inputFileSize = ftell(fh);
	if(inputFileSize == -1) {
		printf("Error: Unable to determine file size\n");
		return(1);
	}

	inbuf = calloc(inputFileSize+5,sizeof(char));	// +1 would work, I do 5 for off-by-one human errors plus a long read.
	outbuf =  calloc(inputFileSize+5,sizeof(char));   
	if(!inbuf || !outbuf) {
		printf("Error: Unable to allocate enough memory for buffers, you must have a wee little system.\n");
		return(137);
	}
	
	if (fseek(fh, 0L, SEEK_SET) != 0) {
		printf("Error: Unable to seek back to beginning of file\n");
		return(1);
	}

	
	fread(inbuf, sizeof(char), inputFileSize, fh);
	if(ferror(fh) != 0) {
		printf("Error reading file.\n");
		return(1);
	}

	fclose(fh);

	// Pass 0: Normalization and rejection

	#ifdef DEBUG
            printf("Pass 0\n");
	#endif

	for(i=0;i<inputFileSize;i++) {
		if(inbuf[i]>96 && inbuf[i]<123) inbuf[i]=inbuf[i]&0b11011111;	// uppercase any lowercase
		if(inbuf[i]==13) inbuf[i]=10;   // convert any CRs to LFs (for non-Unix-y systems)
		if(!(inbuf[i] == 10 || (inbuf[i] > 64 && inbuf[i] < 91))) {
			printf("Non-letter character '%c' in file at byte offset %i, exiting.\n", inbuf[i], i);
			return(1);
		}
		
		#ifdef DEBUG
                printf("%c",inbuf[i]);
		#endif
	}

	// Pass 1: Pack into one string

	#ifdef DEBUG
            printf("\n\nPass 1\n");
            printf(" entering with buffersize %ld, ", inputFileSize);
	#endif

    for(i=0;i<inputFileSize;i++) {
    	if(inbuf[i]==10) {
    		memmove(&inbuf[i],&inbuf[i+1],inputFileSize-i);   // all these memory ops freaking you out?
    		inputFileSize--;
    	}
    }

	#ifdef DEBUG
            printf(" exiting with %ld.\n", inputFileSize);
            printf(" Buffer: ");
            for(i=0;i<inputFileSize;i++) printf("%c",inbuf[i]);
            printf("\n");
	#endif

    // Pass 2: Bitwise pack

    #ifdef DEBUG

            printf("\n\nPass 2\n");

	#endif

    for(i=0;i<inputFileSize;i++) {	
    	current=inbuf[i];
    	#ifdef DEBUG
    		printf("\n%c",current);
    	#endif
	   	for(j=7;j>-1;j--) { // inner loop points to the intrabyte source bit
	   		if(j<5) {  //  we can shorten this loop, but it's a good explainer
		   		putBit(outbuf,(current>>j)&1,bitPointer++);
	   		}
	   	}

	}

 	#ifdef DEBUG

 		// Output result in hex

        printf("\n\nWrite out\n");
        printf("Number of bits to write: %ld\n",bitPointer);

	    for(i=0;i<bitPointer>>3;i++) {
	    	printf("%02hhX",outbuf[i]);
	    	if(i%37==0 && i>1) printf("\n");
	    }

	    printf("\n");

   	#endif

	fh = fopen(argv[2],"wb");

	if(!fh) {
		printf("Error: Unable to open output file\n");
		return(1);
	}


	if(!fwrite(outbuf,1+(bitPointer>>3),1,fh)) {
		printf("Error: Unable to write to output file\n");
		return(1);
	}

	fclose(fh);

	free(inbuf);
	free(outbuf);
	return(0);

}

