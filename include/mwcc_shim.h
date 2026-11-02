#ifndef MWCC_SHIM_H
#define MWCC_SHIM_H

//
// They are not program code or data:
// the link discards both sections by name, and nothing refers to them.
//
// Heap-image shims: mwcc makes every constant aggregate initializer an
// anonymous object (`@N`) and writes it out even when nothing uses it, so an
// image goes to .mwccshim by wrapping its inline function in
//   #pragma section mwccshim begin
//   ...
//   #pragma section mwccshim end
// (the section in force when the initializer is parsed counts; an inline
// member body is parsed at the end of its class).
//
// Session shims: a static function compiled for its effect on mwcc's
// session state goes to .mwccshim.text with MWCC_SHIM_TEXT.
#pragma define_section mwccshim ".mwccshim" far_absolute RW
#pragma define_section mwccshimtext ".mwccshim.text" far_absolute RX
#define MWCC_SHIM_TEXT __attribute__((section(".mwccshim.text")))

#endif
