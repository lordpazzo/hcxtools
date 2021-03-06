#define _GNU_SOURCE
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <pcap.h>
#include <sys/stat.h>
#include <stdio_ext.h>
#include "common.h"

/*===========================================================================*/
/* globale Variablen */

hcx_t *hcxdata = NULL;
/*===========================================================================*/
size_t chop(char *buffer,  size_t len)
{
char *ptr = buffer +len -1;

while (len) {
	if (*ptr != '\n') break;
	*ptr-- = 0;
	len--;
	}

while (len) {
	if (*ptr != '\r') break;
	*ptr-- = 0;
	len--;
	}
return len;
}
/*---------------------------------------------------------------------------*/
int fgetline(FILE *inputstream, size_t size, char *buffer)
{
if (feof(inputstream)) return -1;
		char *buffptr = fgets (buffer, size, inputstream);

	if (buffptr == NULL) return -1;

	size_t len = strlen(buffptr);
	len = chop(buffptr, len);

return len;
}
/*===========================================================================*/
unsigned long long int getreplaycount(uint8_t *eapdata)
{
eap_t *eap;
unsigned long long int replaycount = 0;

eap = (eap_t*)(uint8_t*)(eapdata);
replaycount = be64toh(eap->replaycount);
return replaycount;
}
/*===========================================================================*/
int writmessagepairhccapx(long int hcxrecords, char *mpname, uint8_t message_pair)
{
hcx_t *zeigerhcx;
long int c;
long int rw = 0;
FILE *fhhcx;

c = 0;
while(c < hcxrecords)
	{
	zeigerhcx = hcxdata +c;
	if(zeigerhcx->message_pair == message_pair)
		{
		if((fhhcx = fopen(mpname, "ab")) == NULL)
			{
			fprintf(stderr, "error opening file %s", mpname);
			return FALSE;
			}
		fwrite(zeigerhcx, HCX_SIZE, 1, fhhcx);
		rw++;
		fclose(fhhcx);
		}
	c++;
	}
printf("%ld records written to %s\n", rw, mpname);
return TRUE;
}
/*===========================================================================*/
int writercnotcheckedhccapx(long int hcxrecords, char *rcnotckeckedname)
{
hcx_t *zeigerhcx;
long int c;
long int rw = 0;
FILE *fhhcx;

c = 0;
while(c < hcxrecords)
	{
	zeigerhcx = hcxdata +c;
	if((zeigerhcx->message_pair &0x80) == 0x80)
		{
		if((fhhcx = fopen(rcnotckeckedname, "ab")) == NULL)
			{
			fprintf(stderr, "error opening file %s", rcnotckeckedname);
			return FALSE;
			}
		fwrite(zeigerhcx, HCX_SIZE, 1, fhhcx);
		rw++;
		fclose(fhhcx);
		}
	c++;
	}
printf("%ld records written to %s\n", rw, rcnotckeckedname);
return TRUE;
}
/*===========================================================================*/
int writerccheckedhccapx(long int hcxrecords, char *rcckeckedname)
{
hcx_t *zeigerhcx;
long int c;
long int rw = 0;
FILE *fhhcx;

c = 0;
while(c < hcxrecords)
	{
	zeigerhcx = hcxdata +c;
	if((zeigerhcx->message_pair &0x80) == 0)
		{
		if((fhhcx = fopen(rcckeckedname, "ab")) == NULL)
			{
			fprintf(stderr, "error opening file %s", rcckeckedname);
			return FALSE;
			}
		fwrite(zeigerhcx, HCX_SIZE, 1, fhhcx);
		rw++;
		fclose(fhhcx);
		}
	c++;
	}
printf("%ld records written to %s\n", rw, rcckeckedname);
return TRUE;
}
/*===========================================================================*/
int writewantessidlenhccapx(long int hcxrecords, int essidlen)
{
hcx_t *zeigerhcx;
long int c;
long int rw = 0;
FILE *fhhcx;


char wantessidlenoutname[PATH_MAX +1];
snprintf(wantessidlenoutname, PATH_MAX, "%d.hccapx", essidlen);

c = 0;
while(c < hcxrecords)
	{
	zeigerhcx = hcxdata +c;
	if(zeigerhcx->essid_len == essidlen)
		{
		if((fhhcx = fopen(wantessidlenoutname, "ab")) == NULL)
			{
			fprintf(stderr, "error opening file %s", wantessidlenoutname);
			return FALSE;
			}
		fwrite(zeigerhcx, HCX_SIZE, 1, fhhcx);
		rw++;
		fclose(fhhcx);
		}
	c++;
	}
printf("%ld records written to %s\n", rw, wantessidlenoutname);
return TRUE;
}
/*===========================================================================*/
int writewlandumpnotforcedhccapx(long int hcxrecords, char *wlandumpforcedname)
{
hcx_t *zeigerhcx;
long int c;
long int rw = 0;
unsigned long long int r;
FILE *fhhcx;

c = 0;
while(c < hcxrecords)
	{
	zeigerhcx = hcxdata +c;
	r = getreplaycount(zeigerhcx->eapol);
	if((memcmp(&mynonce, zeigerhcx->nonce_ap, 32) != 0) && (r != MYREPLAYCOUNT))
		{

		if((fhhcx = fopen(wlandumpforcedname, "ab")) == NULL)
			{
			fprintf(stderr, "error opening file %s", wlandumpforcedname);
			return FALSE;
			}
		fwrite(zeigerhcx, HCX_SIZE, 1, fhhcx);
		rw++;
		fclose(fhhcx);
		}
	c++;
	}
printf("%ld records written to %s\n", rw, wlandumpforcedname);
return TRUE;
}
/*===========================================================================*/
int writewlandumpforcedhccapx(long int hcxrecords, char *wlandumpforcedname)
{
hcx_t *zeigerhcx;
long int c;
long int rw = 0;
unsigned long long int r;
FILE *fhhcx;

c = 0;
while(c < hcxrecords)
	{
	zeigerhcx = hcxdata +c;
	r = getreplaycount(zeigerhcx->eapol);
	if((memcmp(&mynonce, zeigerhcx->nonce_ap, 32) == 0) && (r == MYREPLAYCOUNT))
		{
		if((fhhcx = fopen(wlandumpforcedname, "ab")) == NULL)
			{
			fprintf(stderr, "error opening file %s", wlandumpforcedname);
			return FALSE;
			}
		fwrite(zeigerhcx, HCX_SIZE, 1, fhhcx);
		rw++;
		fclose(fhhcx);
		}
	c++;
	}
printf("%ld records written to %s\n", rw, wlandumpforcedname);
return TRUE;
}
/*===========================================================================*/
int writemacaplisthccapx(long int hcxrecords, char *aplistname, char *apoutname)
{
adr_t mac;
hcx_t *zeigerhcx;
FILE *fhaplist;
FILE *fhhcx;
unsigned long long int mac_ap;
long int c;
long int rw = 0;
int len;

char linein[14];

if((fhaplist = fopen(aplistname, "r")) == NULL)
	{
	fprintf(stderr, "error opening file %s", aplistname);
	return FALSE;
	}

while((len = fgetline(fhaplist, 14, linein)) != -1)
	{
	if(len != 12)
		continue;

	mac_ap = strtoul(linein, NULL, 16);
	mac.addr[5] = mac_ap & 0xff;
	mac.addr[4] = (mac_ap >> 8) & 0xff;
	mac.addr[3] = (mac_ap >> 16) & 0xff;
	mac.addr[2] = (mac_ap >> 24) & 0xff;
	mac.addr[1] = (mac_ap >> 32) & 0xff;
	mac.addr[0] = (mac_ap >> 40) & 0xff;
	c = 0;
	while(c < hcxrecords)
		{
		zeigerhcx = hcxdata +c;
		if(memcmp(&mac.addr, zeigerhcx->mac_ap.addr, 6) == 0)
			{
			if((fhhcx = fopen(apoutname, "ab")) == NULL)
				{
				fprintf(stderr, "error opening file %s", apoutname);
				return FALSE;
				}
			fwrite(zeigerhcx, HCX_SIZE, 1, fhhcx);
			rw++;
			fclose(fhhcx);
			}
		c++;
		}
	}

printf("%ld records written to %s\n", rw, apoutname);
fclose(fhaplist);
return TRUE;
}
/*===========================================================================*/
int writesearchouihccapx(long int hcxrecords, unsigned long long int oui)
{
hcx_t *zeigerhcx;
FILE *fhhcx;
long int c;
long int rw = 0;
adr_t mac;

char macoutname[PATH_MAX +1];

snprintf(macoutname, PATH_MAX, "%06llx.hccapx", oui);

mac.addr[2] = oui & 0xff;
mac.addr[1] = (oui >> 8) & 0xff;
mac.addr[0] = (oui >> 16) & 0xff;

c = 0;
while(c < hcxrecords)
	{
	zeigerhcx = hcxdata +c;
	if(memcmp(&mac.addr, zeigerhcx->mac_ap.addr, 3) == 0)
		{
		if((fhhcx = fopen(macoutname, "ab")) == NULL)
			{
			fprintf(stderr, "error opening file %s", macoutname);
			return FALSE;
			}
		fwrite(zeigerhcx, HCX_SIZE, 1, fhhcx);
		rw++;
		fclose(fhhcx);
		}
	c++;
	}
printf("%ld records written\n", rw);
return TRUE;
}
/*===========================================================================*/
int writesearchmacstahccapx(long int hcxrecords, unsigned long long int mac_sta)
{
hcx_t *zeigerhcx;
FILE *fhhcx;
long int c;
long int rw = 0;
adr_t mac;

char macoutname[PATH_MAX +1];

snprintf(macoutname, PATH_MAX, "%012llx.hccapx", mac_sta);

mac.addr[5] = mac_sta & 0xff;
mac.addr[4] = (mac_sta >> 8) & 0xff;
mac.addr[3] = (mac_sta >> 16) & 0xff;
mac.addr[2] = (mac_sta >> 24) & 0xff;
mac.addr[1] = (mac_sta >> 32) & 0xff;
mac.addr[0] = (mac_sta >> 40) & 0xff;

c = 0;
while(c < hcxrecords)
	{
	zeigerhcx = hcxdata +c;
	if(memcmp(&mac.addr, zeigerhcx->mac_sta.addr, 6) == 0)
		{
		if((fhhcx = fopen(macoutname, "ab")) == NULL)
			{
			fprintf(stderr, "error opening file %s", macoutname);
			return FALSE;
			}
		fwrite(zeigerhcx, HCX_SIZE, 1, fhhcx);
		rw++;
		fclose(fhhcx);
		}
	c++;
	}
printf("%ld records written\n", rw);
return TRUE;
}
/*===========================================================================*/
int writesearchmacaphccapx(long int hcxrecords, unsigned long long int mac_ap)
{
hcx_t *zeigerhcx;
FILE *fhhcx;
long int c;
long int rw = 0;
adr_t mac;

char macoutname[PATH_MAX +1];

snprintf(macoutname, PATH_MAX, "%012llx.hccapx", mac_ap);

mac.addr[5] = mac_ap & 0xff;
mac.addr[4] = (mac_ap >> 8) & 0xff;
mac.addr[3] = (mac_ap >> 16) & 0xff;
mac.addr[2] = (mac_ap >> 24) & 0xff;
mac.addr[1] = (mac_ap >> 32) & 0xff;
mac.addr[0] = (mac_ap >> 40) & 0xff;

c = 0;
while(c < hcxrecords)
	{
	zeigerhcx = hcxdata +c;
	if(memcmp(&mac.addr, zeigerhcx->mac_ap.addr, 6) == 0)
		{
		if((fhhcx = fopen(macoutname, "ab")) == NULL)
			{
			fprintf(stderr, "error opening file %s", macoutname);
			return FALSE;
			}
		fwrite(zeigerhcx, HCX_SIZE, 1, fhhcx);
		rw++;
		fclose(fhhcx);
		}
	c++;
	}
printf("%ld records written\n", rw);
return TRUE;
}
/*===========================================================================*/
int writesearchessidhccapx(long int hcxrecords, char *essidname)
{
hcx_t *zeigerhcx;
FILE *fhhcx;
long int c;
long int rw = 0;
char essidoutname[PATH_MAX +1];

char essidstr[34];

snprintf(essidoutname, PATH_MAX, "%s.hccapx", essidname);
c = 0;
while(c < hcxrecords)
	{
	zeigerhcx = hcxdata +c;
	memset(&essidstr, 0, 34);
	memcpy(&essidstr, zeigerhcx->essid, zeigerhcx->essid_len);
	if(strstr(essidstr, essidname) != NULL)
		{
		if((fhhcx = fopen(essidoutname, "ab")) == NULL)
			{
			fprintf(stderr, "error opening file %s", essidoutname);
			return FALSE;
			}
		fwrite(zeigerhcx, HCX_SIZE, 1, fhhcx);
		rw++;
		fclose(fhhcx);
		}
	c++;
	}
printf("%ld records written\n", rw);
return TRUE;
}
/*===========================================================================*/
int writesearchessidxhccapx(long int hcxrecords, char *essidxname)
{
hcx_t *zeigerhcx;
FILE *fhhcx;
long int c;
long int rw = 0;
char essidxoutname[PATH_MAX +1];

char essidstr[34];

snprintf(essidxoutname, PATH_MAX, "%s.hccapx", essidxname);
c = 0;
while(c < hcxrecords)
	{
	zeigerhcx = hcxdata +c;
	memset(&essidstr, 0, 34);
	memcpy(&essidstr, zeigerhcx->essid, zeigerhcx->essid_len);
	if(strcmp(essidstr, essidxname) == 0)
		{
		if((fhhcx = fopen(essidxoutname, "ab")) == NULL)
			{
			fprintf(stderr, "error opening file %s", essidxoutname);
			return FALSE;
			}
		fwrite(zeigerhcx, HCX_SIZE, 1, fhhcx);
		rw++;
		fclose(fhhcx);
		}
	c++;
	}
printf("%ld records written\n", rw);
return TRUE;
}
int writeessidhccapx(long int hcxrecords)
{
hcx_t *zeigerhcx;
FILE *fhhcx;
long int c;
long int rw = 0;
int cei, ceo;

const char digit[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

char hcxoutname[PATH_MAX +1];

c = 0;
while(c < hcxrecords)
	{
	zeigerhcx = hcxdata +c;
	ceo = 0;
	for (cei = 0; cei < zeigerhcx->essid_len; cei++)
		{
		hcxoutname[ceo] = digit[(zeigerhcx->essid[cei] & 0xff) >> 4];
		ceo++;
		hcxoutname[ceo] = digit[zeigerhcx->essid[cei] & 0x0f];
		ceo++;
		}
	hcxoutname[ceo] = 0;
	strcat(&hcxoutname[ceo], ".hccapx");
	if((fhhcx = fopen(hcxoutname, "ab")) == NULL)
		{
		fprintf(stderr, "error opening file %s", hcxoutname);
		return FALSE;
		}
	fwrite(zeigerhcx, HCX_SIZE, 1, fhhcx);
	rw++;
	fclose(fhhcx);
	c++;
	}
printf("%ld records written\n", rw);
return TRUE;
}
/*===========================================================================*/
void oui2hxoutname(char ssid[13], unsigned char *p)
{
sprintf(ssid, "%02x%02x%02x.hccapx",p[0],p[1],p[2]);
return;
}
/*===========================================================================*/
void mac2hxoutname(char ssid[13], unsigned char *p)
{
sprintf(ssid, "%02x%02x%02x%02x%02x%02x.hccapx",p[0],p[1],p[2],p[3],p[4],p[5]);
return;
}
/*===========================================================================*/
int writeouiaphccapx(long int hcxrecords)
{
hcx_t *zeigerhcx;
FILE *fhhcx;
long int c;
long int rw = 0;
char hcxoutname[PATH_MAX +1];

c = 0;
while(c < hcxrecords)
	{
	zeigerhcx = hcxdata +c;
	oui2hxoutname(hcxoutname, zeigerhcx->mac_ap.addr);
	if((fhhcx = fopen(hcxoutname, "ab")) == NULL)
		{
		fprintf(stderr, "error opening file %s", hcxoutname);
		return FALSE;
		}
	fwrite(zeigerhcx, HCX_SIZE, 1, fhhcx);
	rw++;
	fclose(fhhcx);
	c++;
	}
printf("%ld records written\n", rw);
return TRUE;
}
/*===========================================================================*/
int writemacstahccapx(long int hcxrecords)
{
hcx_t *zeigerhcx;
FILE *fhhcx;
long int c;
long int rw = 0;

char hcxoutname[PATH_MAX +1];

c = 0;
while(c < hcxrecords)
	{
	zeigerhcx = hcxdata +c;
	mac2hxoutname(hcxoutname, zeigerhcx->mac_sta.addr);
	if((fhhcx = fopen(hcxoutname, "ab")) == NULL)
		{
		fprintf(stderr, "error opening file %s", hcxoutname);
		return FALSE;
		}
	fwrite(zeigerhcx, HCX_SIZE, 1, fhhcx);
	rw++;
	fclose(fhhcx);
	c++;
	}
printf("%ld records written\n", rw);
return TRUE;
}
/*===========================================================================*/
int writemacaphccapx(long int hcxrecords)
{
hcx_t *zeigerhcx;
FILE *fhhcx;
long int c;
long int rw = 0;

char hcxoutname[PATH_MAX +1];

c = 0;
while(c < hcxrecords)
	{
	zeigerhcx = hcxdata +c;
	mac2hxoutname(hcxoutname, zeigerhcx->mac_ap.addr);
	if((fhhcx = fopen(hcxoutname, "ab")) == NULL)
		{
		fprintf(stderr, "error opening file %s", hcxoutname);
		return FALSE;
		}
	fwrite(zeigerhcx, HCX_SIZE, 1, fhhcx);
	rw++;
	fclose(fhhcx);
	c++;
	}

printf("%ld records written\n", rw);
return TRUE;
}
/*===========================================================================*/
int readhccapx(char *hcxinname)
{
struct stat statinfo;
FILE *fhhcx;
long int hcxsize = 0;

if(hcxinname == NULL)
	return FALSE;

if(stat(hcxinname, &statinfo) != 0)
	{
	fprintf(stderr, "can't stat %s\n", hcxinname);
	return FALSE;
	}

if((statinfo.st_size % HCX_SIZE) != 0)
	{
	fprintf(stderr, "file corrupt\n");
	return FALSE;
	}

if((fhhcx = fopen(hcxinname, "rb")) == NULL)
	{
	fprintf(stderr, "error opening file %s", hcxinname);
	return FALSE;
	}

hcxdata = malloc(statinfo.st_size);
if(hcxdata == NULL)	
		{
		fprintf(stderr, "out of memory to store hccapx data\n");
		return FALSE;
		}

hcxsize = fread(hcxdata, 1, statinfo.st_size +HCX_SIZE, fhhcx);
if(hcxsize != statinfo.st_size)	
	{
	fprintf(stderr, "error reading hccapx file %s", hcxinname);
	return FALSE;
	}
fclose(fhhcx);

printf("%ld records read from %s\n", hcxsize / HCX_SIZE, hcxinname);
return hcxsize / HCX_SIZE;
}
/*===========================================================================*/
static void usage(char *eigenname)
{
printf("%s %s (C) %s ZeroBeat\n"
	"usage: %s <options>\n"
	"\n"
	"options:\n"
	"-i <file>     : input hccapx file\n"
	"-p <path>     : change directory for outputfiles\n"
	"-a            : output file by mac_ap's\n"
	"-s            : output file by mac_sta's\n"
	"-o            : output file by vendor's (oui)\n"
	"-e            : output file by essid's\n"
	"-E <essid>    : output file by part of essid name\n"
	"-X <essid>    : output file by essid name (exactly)\n"
	"-x <digit>    : output by essid len (1 <= 32)\n"
	"-A <mac_ap>   : output file by single mac_ap\n"
	"-S <mac_sta>  : output file by single mac_sta\n"
	"-O <oui>      : output file by single vendor (oui)\n"
	"-L <mac_list> : input list containing mac_ap's (need -l)\n"
	"              : format of mac_ap's each line: 112233445566\n"
	"-l <file>     : output file (hccapx) by mac_list (need -L)\n"
	"-w <file>     : write only wlandump forced to hccapx file\n"
	"-W <file>     : write only not wlandump forced to hccapx file\n"
	"-r <file>     : write only replaycount checked to hccapx file\n"
	"-R <file>     : write only not replaycount checked to hccapx file\n"
	"-0 <file>     : write only MESSAGE_PAIR_M12E2 to hccapx file\n"
	"-1 <file>     : write only MESSAGE_PAIR_M14E4 to hccapx file\n"
	"-2 <file>     : write only MESSAGE_PAIR_M32E2 to hccapx file\n"
	"-3 <file>     : write only MESSAGE_PAIR_M32E3 to hccapx file\n"
	"-4 <file>     : write only MESSAGE_PAIR_M34E3 to hccapx file\n"
	"-5 <file>     : write only MESSAGE_PAIR_M34E4 to hccapx file\n"
	"-h            : this help\n"
	"\n", eigenname, VERSION, VERSION_JAHR, eigenname);
exit(EXIT_FAILURE);
}
/*===========================================================================*/
int main(int argc, char *argv[])
{
int auswahl;
int mode = 0;
int retchdir = 0;
int wantessidlen = 1;
long int hcxorgrecords = 0;
unsigned long long int mac_ap = 0;
unsigned long long int mac_sta = 0;
unsigned long long int oui = 0;
uint8_t message_pair = 0;

char *eigenname = NULL;
char *eigenpfadname = NULL;
char *hcxinname = NULL;
char *essidname = NULL;
char *essidxname = NULL;
char *aplistname = NULL;
char *apoutname = NULL;
char *wlandumpforcedname = NULL;
char *rccheckedname = NULL;
char *rcnotcheckedname = NULL;
char *mpname = NULL;
char *workingdirname;
char *wdres;
char workingdir[PATH_MAX +1];

eigenpfadname = strdupa(argv[0]);
eigenname = basename(eigenpfadname);

setbuf(stdout, NULL);
wdres = getcwd(workingdir, PATH_MAX);
if(wdres != NULL)
	workingdirname = workingdir;
while ((auswahl = getopt(argc, argv, "i:A:S:O:E:X:x:p:l:L:w:W:r:R:0:1:2:3:4:5:asoeh")) != -1)
	{
	switch (auswahl)
		{
		case 'i':
		hcxinname = optarg;
		break;

		case 'a':
		mode = 'a';
		break;

		case 's':
		mode = 's';
		break;

		case 'o':
		mode = 'o';
		break;

		case 'e':
		mode = 'e';
		break;

		case 'p':
		workingdirname = optarg;
		break;


		case 'E':
		essidname = optarg;
		if(strlen(essidname) > 32)
			{
			fprintf(stderr, "essid > 32\n");
			exit(EXIT_FAILURE);
			}
		break;

		case 'X':
		essidxname = optarg;
		if(strlen(essidxname) > 32)
			{
			fprintf(stderr, "essid > 32\n");
			exit(EXIT_FAILURE);
			}
		break;

		case 'x':
		wantessidlen = strtoul(optarg, NULL, 10);
		if((wantessidlen < 1) || (wantessidlen > 32))
			{
			fprintf(stderr, "essid > 32\n");
			exit(EXIT_FAILURE);
			}
	
		mode = 'x';
		break;

		case 'A':
		if(strlen(optarg) != 12)
			{
			fprintf(stderr, "error wrong mac size %s (need 112233aabbcc)\n", optarg);
			exit(EXIT_FAILURE);
			}
		mac_ap = strtoul(optarg, NULL, 16);
		mode = 'A';
		break;

		case 'S':
		if(strlen(optarg) != 12)
			{
			fprintf(stderr, "error wrong mac size %s (need 112233aabbcc)\n", optarg);
			exit(EXIT_FAILURE);
			}
		mac_sta = strtoul(optarg, NULL, 16);
		mode = 'S';
		break;

		case 'O':
		if(strlen(optarg) != 6)
			{
			fprintf(stderr, "error wrong oui size %s (need 1122aa)\n", optarg);
			exit(EXIT_FAILURE);
			}
		oui = strtoul(optarg, NULL, 16);
		mode = 'O';
		break;

		case 'l':
		apoutname = optarg;
		break;

		case 'L':
		aplistname = optarg;
		mode = 'L';
		break;

		case 'w':
		wlandumpforcedname = optarg;
		mode = 'w';
		break;

		case 'W':
		wlandumpforcedname = optarg;
		mode = 'W';
		break;

		case 'r':
		rccheckedname = optarg;
		mode = 'r';
		break;

		case 'R':
		rcnotcheckedname = optarg;
		mode = 'R';
		break;

		case '0':
		mpname = optarg;
		mode = 'M';
		message_pair = MESSAGE_PAIR_M12E2;
		break;

		case '1':
		mpname = optarg;
		mode = 'M';
		message_pair = MESSAGE_PAIR_M14E4;
		break;

		case '2':
		mpname = optarg;
		mode = 'M';
		message_pair = MESSAGE_PAIR_M32E2;
		break;

		case '3':
		mpname = optarg;
		mode = 'M';
		message_pair = MESSAGE_PAIR_M32E3;
		break;

		case '4':
		mpname = optarg;
		mode = 'M';
		message_pair = MESSAGE_PAIR_M34E3;
		break;

		case '5':
		mpname = optarg;
		mode = 'M';
		message_pair = MESSAGE_PAIR_M34E4;
		break;

		case 'h':
		usage(eigenname);
		break;

		default:
		usage(eigenname);
		break;
		}
	}

hcxorgrecords = readhccapx(hcxinname);
if(hcxorgrecords == 0)
	return EXIT_SUCCESS;

retchdir = chdir(workingdirname);
if(retchdir != 0)
	fprintf(stderr, " couldn't change working directory to %s\nusing %s\n", workingdirname, workingdir); 

if(mode == 'a')
	writemacaphccapx(hcxorgrecords);

else if(mode == 's')
	writemacstahccapx(hcxorgrecords);

else if(mode == 'o')
	writeouiaphccapx(hcxorgrecords);

else if(mode == 'e')
	writeessidhccapx(hcxorgrecords);


else if(mode == 'A')
	writesearchmacaphccapx(hcxorgrecords, mac_ap);

else if(mode == 'S')
	writesearchmacstahccapx(hcxorgrecords, mac_sta);

else if(mode == 'O')
	writesearchouihccapx(hcxorgrecords, oui);

else if(essidname != NULL)
	writesearchessidhccapx(hcxorgrecords, essidname);

else if(mode == 'x')
	writewantessidlenhccapx(hcxorgrecords, wantessidlen);

else if(essidxname != NULL)
	writesearchessidxhccapx(hcxorgrecords, essidxname);

else if(mode == 'L')
	{
	if((aplistname != NULL) && (apoutname != 0))
		writemacaplisthccapx(hcxorgrecords, aplistname, apoutname);
	else
		{
		fprintf(stderr, "need -L (input list of mac_ap's to strip and -l output file (hccapx)\n");
		exit(EXIT_FAILURE);
		}
	}

else if(mode == 'w')
	{
	if(wlandumpforcedname != NULL)
		writewlandumpforcedhccapx(hcxorgrecords, wlandumpforcedname);
	}

else if(mode == 'W')
	{
	if(wlandumpforcedname != NULL)
		writewlandumpnotforcedhccapx(hcxorgrecords, wlandumpforcedname);
	}

else if(mode == 'r')
	{
	if(rccheckedname != NULL)
		writerccheckedhccapx(hcxorgrecords, rccheckedname);
	}

else if(mode == 'R')
	{
	if(rcnotcheckedname != NULL)
		writercnotcheckedhccapx(hcxorgrecords, rcnotcheckedname);
	}

else if(mode == 'M')
	{
	if(mpname != NULL)
		writmessagepairhccapx(hcxorgrecords, mpname, message_pair);
	}


if(hcxdata != NULL)
	free(hcxdata);

return EXIT_SUCCESS;
}
