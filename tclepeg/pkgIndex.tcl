set file [file join $dir tclepeg[info sharedlibextension]]
package ifneeded tclepeg 0.4 "load $file"
 
