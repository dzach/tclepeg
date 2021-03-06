/*
* tclepeg.c -- A fast JPEG resize (reduction) Tcl C extension based on the epeg library
* For a source repository of epeg look in : 
* 	http://maemo.gitorious.org/maemo-af/epeg
* 	http://svn.enlightenment.org/svn/e/OLD/epeg/
* 
* Adapted to tcl by Dimitrios Zachariadis
* BSD licensed
*/
#include <tcl.h>
#include <stdlib.h>
#include <string.h>
#include "Epeg.h"

static int 
Tclepeg_Cmd(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	if(objc < 2) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj("wrong # args. Should be epeg ?-width width? ?-height height? ?-quality quality? ?-max maxdimension? jpegdata", -1));
		return TCL_ERROR;
	}
	int i, W, H, w=0, h=0, q=50, m=64, sizei, sizeo;
	unsigned char *imi, *imo, *imo1, *opt, *val;
	Epeg_Image *im;
	
	// get input image
	imi = Tcl_GetByteArrayFromObj(objv[objc-1], &sizei);
	// input image in memory
	if ( (im = epeg_memory_open(imi,sizei)) == NULL)
	{
		Tcl_SetObjResult(interp, Tcl_NewStringObj("epeg: could not open image in memory", -1));
		return TCL_ERROR;
	}
	
	// set user options
	for (i=1; i<objc-2; i++)
	{
		opt = Tcl_GetString(objv[i]);
		if (strncmp(opt, "-w", 2) == 0)
		{
			w = atof(Tcl_GetString(objv[++i]));
		}
		else if (strncmp(opt, "-h", 2) == 0)
		{
			h = atof(Tcl_GetString(objv[++i]));
		}
		else if (strncmp(opt, "-q", 2) == 0)
		{
			q = atof(Tcl_GetString(objv[++i]));
		}
		else if (strncmp(opt, "-m", 2) == 0)
		{
			m = atof(Tcl_GetString(objv[++i]));
		}
		else
		{
			fprintf(stderr,"option %s is unknown\n",Tcl_GetString(objv[i]));
		}
	}
	/* resize image to max dimension whlile keeping the aspect ratio
	 * but only if neither width nor height have been set by user
	 * If -m is not set, then use the default
	*/
	if (w == 0 || h == 0)
	{
		// get input image dimensions
		epeg_size_get(im, &W, &H);
		if (W > H) {
			w = m;
			h = m * H / W;
		} else {
			h = m;
			w = m * W / H;
		}
	}
	epeg_decode_size_set           (im, w, h);
	epeg_quality_set               (im, q);
	epeg_thumbnail_comments_enable (im, 1);
	epeg_memory_output_set         (im, &imo, &sizeo);
	
	// encoding did not succeed
	if (epeg_encode(im) != 0)
	{
		Tcl_SetObjResult(interp, Tcl_NewStringObj("epeg: encode failed", -1));
		epeg_close(im);
		return TCL_ERROR;
	}
	
	// copy output image to a tcl byte array
	Tcl_Obj *timo = Tcl_NewByteArrayObj(imo, sizeo);
	// free image memory
	epeg_close                     (im);
	// epeg_close does n ot free output memory, so do it here
	free(imo);
	// return output image
	Tcl_SetObjResult(interp, timo);
	return TCL_OK;
}
	
/*
* Tclepeg_Init -- Called when Tcl loads the extension.
*/
int DLLEXPORT
Tclepeg_Init(Tcl_Interp *interp)
{
	if (Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL) {
		return TCL_ERROR;
	}
	/* changed this to check for an error - GPS */
	if (Tcl_PkgProvide(interp, "tclepeg", "0.4") == TCL_ERROR) {
		return TCL_ERROR;
	}
	Tcl_CreateObjCommand(interp, "epeg", Tclepeg_Cmd, NULL, NULL);
	return TCL_OK;
}

