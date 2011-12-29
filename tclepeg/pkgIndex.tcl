set file [file join $dir tclepeg[info sharedlibextension]]
package ifneeded tclepeg 0.3 "load $file"
 
