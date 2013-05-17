/*
 * DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
 * Version 2, December 2004 
 *
 * Copyright (C) 2012-2013 Sebastien Tricaud <sebastien@honeynet.org> 
 *
 * Everyone is permitted to copy and distribute verbatim or modified 
 * copies of this license document, and changing it is allowed as long 
 * as the name is changed. 
 *
 * DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
 * TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION 
 *
 *  0. You just DO WHAT THE FUCK YOU WANT TO.
 *
 **********************************************************************************
 *
 * Implements the browser emulation of uri handling in some particular cases.
 * Naturally, this kind of url will be handled by faup this way:
 * $ faup http://www.example.com?foo:bar |cut -d, -f6
 * com?foo
 *
 * While we all recognized this is not a problem in faup, it is still a problem
 * since browsers such as Firefox will replace www.example.com?foo:bar with
 * www.example.com/?foo:bar
 * 
 * It can be processed by faup to retrieve the TLD properly:
 * $ faup http://www.example.com/?foo:bar |cut -d, -f6
 * com
 *
 * Faup is here to remove the hassle from getting a TLD, since browsers behave that
 * way, faup can emulate their behavior.
 * For the moment, only firefox is implemented. The user tool must provide a flag
 * that can take two strings: firefox or any.
 * To emulate firefox, simply type faup -b firefox
 * To emulate any kind of browser, simply type faup -b any
 * 'any' will summarize the best of the different behaviors among the different browsers
 * 
 * A funny thing would be to be able to recognize the browser of our client is using just
 * by looking at DNS requests :)
 *
 * Resources:
 * http://mxr.mozilla.org/mozilla-central/source/netwerk/base/public/nsIURI.idl 
 * http://blogs.msdn.com/b/ie/archive/2005/08/15/452006.aspx
 *
 * Security issues to consider:
 * http://archive.cert.uni-stuttgart.de/bugtraq/2005/02/msg00123.html
 */

/* For internet explorer, the following URIs are all equivalent:
 *
 *  http://www.example.com/sample/
 *  http://www.EXAMPLE.com/sample/#
 *  Http://www.ex%41mple.com/s%61mple/
 *  http://www.example.com/sample/arbitrary/.%2e/
 */
char *faup_emulation_internet_explorer(faup_handler_t const* fh)
{


}

char *faup_emulation_safari(faup_handler_t const* fh)
{


}

char *faup_emulation_firefox(faup_handler_t const* fh)
{


}



