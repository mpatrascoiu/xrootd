/******************************************************************************/
/******************************************************************************/
/*                         i n c l u d e   f i l e s                          */
/******************************************************************************/

#include <algorithm>
#include <string.h>

#include "XrdVersion.hh"

#include "XrdOuc/XrdOucName2Name.hh"
#include "XrdOuc/XrdOucTokenizer.hh"
#include "XrdSys/XrdSysError.hh"


/******************************************************************************/
/*                      C l a s s   D e f i n i t i o n                       */
/******************************************************************************/

XrdVERSIONINFO(XrdOucgetName2Name, XrdN2Neos);

class XrdOucN2Neos : public XrdOucName2Name
{
public:

virtual int lfn2pfn(const char* lfn, char* buff, int blen);

virtual int lfn2rfn(const char* lfn, char* buff, int blen) { return -ENOTSUP; }

virtual int pfn2lfn(const char* pfn, char* buff, int blen) { return -ENOTSUP; }

            XrdOucN2Neos(XrdSysError *erp, const char *lval);

virtual    ~XrdOucN2Neos()  { if (localVal) free(localVal); }

private:

XrdSysError *eDest;
char        *localVal;
int          localValLen;
};


/******************************************************************************/
/*                        I m p l e m e n t a t i o n                         */
/******************************************************************************/
/******************************************************************************/
/*                           C o n s t r u c t o r                            */
/******************************************************************************/

XrdOucN2Neos::XrdOucN2Neos(XrdSysError *erp, const char *lval)
{
  eDest = erp;

  // Save the local value
  //
  if (!lval) { localVal = 0; localValLen = 0; }
    else if (!(localValLen = strlen(localVal))) localVal = 0;
            else { localVal = strdup(lval); }
}

/******************************************************************************/
/*                               l f n 2 p f n                                */
/******************************************************************************/

int XrdOucN2Neos::lfn2pfn(const char *lfn, char *buff, int blen)
{
  char msg[4096];
  int flen = strlen(lfn) + localValLen + 1;

  // If we have a local value then append it as opaque information
  // to the file name (make sure it fits). Otherwise, return the file name.
  //
  if (localVal)
     { if ((localValLen + flen + 1) >= blen )
          { snprintf(msg, flen,
                     "filename with CGI info is too long: %s", localVal);
            return eDest->Emsg("N2Neos", -ENAMETOOLONG, msg);
          }
       snprintf(buff, flen, "%s?lval=%s", lfn, localVal);
     } else { strncpy(buff, lfn, blen); }

  return 0;
}

/******************************************************************************/
/*                    X r d O u c g e t N a m e 2 N a m e                     */
/******************************************************************************/

XrdOucName2Name *XrdOucgetName2Name(XrdOucgetName2NameArgs)
{
  char *p = parms ? strdup(parms) : 0;
  char *val;

  // Process the config parameter value
  //
  XrdOucTokenizer toks(p);
  toks.GetLine();

  val = toks.GetToken();
  if (!val || !(*val))
    { eDest->Emsg("N2Neos", "No parameter supplied.");
      return 0;
    }

  return new XrdOucN2Neos(eDest, val);
}
