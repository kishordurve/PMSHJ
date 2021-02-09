/*----------------------
   TSCLIB.H header file
  ----------------------*/

#ifdef __cplusplus
#define EXPORT extern "C" __declspec (dllexport)
#else
#define EXPORT __declspec (dllexport)
#endif

EXPORT int CALLBACK openport(char*);

EXPORT int CALLBACK closeport();

EXPORT int CALLBACK sendcommand (char*);

EXPORT int CALLBACK setup (char *,char *,char *,char *,char *,char *,char *);

EXPORT int CALLBACK downloadpcx (char *filename,char *image_name);

EXPORT int CALLBACK barcode (char *,char *,char *,char *,char *,char *,char *,char *,char *);

EXPORT int CALLBACK printerfont (char *,char *,char *,char *,char *,char *,char *);

EXPORT int CALLBACK clearbuffer();

EXPORT int CALLBACK printlabel(char *, char *);

EXPORT int CALLBACK formfeed();

EXPORT int CALLBACK nobackfeed();

EXPORT BOOL CALLBACK isready();

EXPORT int CALLBACK windowsfont(int x, int y, int fontheight, int rotation, int fontstyle, int fontunderline, char *szFaceName, char *content);

EXPORT int CALLBACK about();

int OutBuffer(char *);
int CreatePort(char *);
