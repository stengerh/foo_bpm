/******************************************************************************
*          DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
*                    Version 2, December 2004
*
* Copyright (C) 2009-2010, Michael Balzer         Email: fraganator@hotmail.com
* Everyone is permitted to copy and distribute verbatim or modified
* copies of this license document, and changing it is allowed as long
* as the name is changed.
*
*            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
*   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
*
*  0. You just DO WHAT THE FUCK YOU WANT TO.
*
* This program is free software. It comes without any warranty, to
* the extent permitted by applicable law. You can redistribute it
* and/or modify it under the terms of the Do What The Fuck You Want
* To Public License, Version 2, as published by Sam Hocevar. See
* http://sam.zoy.org/wtfpl/COPYING for more details.
*
* If the code is redistributed or modified, please drop me an email letting me
* know how the code is being used. It's nice to know where it ends up :)
*
*******************************************************************************
*
* foo_bpm.cpp - A foobar2000 component for automatically detecting a song's BPM
*
* REFERENCES:
* BPM estimation algorithm based on:
* Tempo and Beat Estimation of Musical Signals - http://ismir2004.ismir.net/proceedings/p032-page-158-paper191.pdf
*
* OTHER REFERENCES:
* [1] Onset Detection Revisited - http://www.dafx.ca/proceedings/papers/p_133.pdf
* [2] A Comparison of Sound Onset Detection Algorithms with Emphasis on Psychoacoustically Motivated Detection Functions - http://www.cogs.susx.ac.uk/users/nc81/research/comparison.pdf
* [3] Window Functions - http://en.wikipedia.org/wiki/Window_function#Window_examples
*
* REVISION HISTORY:
* Date       | Version      | Description
* -----------------------------------------------------------------------------
* 21/12/2009 | 0.1.0        | Initial release
* -----------------------------------------------------------------------------
* 24/12/2009 | 0.1.1        | Refactored code (with processing time decrease)
*            |              | Initial source code release
* -----------------------------------------------------------------------------
* 31/12/2009 | 0.2.0        | Added confirmation to rescan already tagged files
*            |              | Added ReplayGain style results dialog
*            |              | Added preferences page (with destination BPM tag)
*            |              | Added manual bpm calculation window
* -----------------------------------------------------------------------------
* 02/01/2010 | 0.2.1        | Miscellaneous bug fixes
* -----------------------------------------------------------------------------
* 07/01/2010 | 0.2.2        | Crash report fix
*            |              | Candidate bpm selection can be mode, mean, median
*            |              | added to preferences
*            |              | Debug output added to preferences
* -----------------------------------------------------------------------------
* 11/01/2010 | 0.2.3        | Updated to foobar2000 1.0 SDK
*            |              | Added double/halve buttons to results dialog
*            |              | Added option to auto write tags after analysis
*            |              | Limit preference range inputs
*            |              | Crash report fix (using info not yet cached)
* -----------------------------------------------------------------------------
* 12/01/2010 | 0.2.4        | Crash report fix (component about message)
* -----------------------------------------------------------------------------
* 19/04/2010 | 0.2.4.1      | Numerous bug fixes
*
******************************************************************************/

#include "stdafx.h"
#include "foo_bpm.h"

DECLARE_COMPONENT_VERSION("BPM Analyser", "0.2.4.1",
"A component for automatically analysing the BPM of audio files.\n\n\
Makes use of the FFTW library (www.fftw.org)\n\n\
References:\n\
Tempo and Beat Estimation of Musical Signals -\nhttp://ismir2004.ismir.net/proceedings/p032-page-158-paper191.pdf \n\n\
Onset Detection Revisited -\nhttp://www.dafx.ca/proceedings/papers/p_133.pdf \n\n\
A Comparison of Sound Onset Detection Algorithms with Emphasis on Psychoacoustically Motivated Detection Functions -\nhttp://www.cogs.susx.ac.uk/users/nc81/research/comparison.pdf \n\n\
Window Functions -\nhttp://en.wikipedia.org/wiki/Window_function#Window_examples \n\n\n\
(c) 2009-2010 Michael Balzer (fraganator@hotmail.com)");

VALIDATE_COMPONENT_FILENAME("foo_bpm.dll");
