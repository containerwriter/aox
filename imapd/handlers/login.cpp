#include "login.h"

#include "../imap.h"


/*!  Constructs an simple Login handler. */

Login::Login()
    : Command()
{
}


/*! Destroys the Login handler. */

Login::~Login()
{
    // nothing
}


/*! Parses arguments for the login command. */

void Login::parse()
{
    n = astring();
    space();
    p = astring();
    end();
}


/*! Verifies that names and passwords match perfectly. */

void Login::execute()
{
    if ( n == "arnt" && p == "trish" )
        imap()->setState( IMAP::Authenticated );
    else if ( n == "ams" )
        error( No, "no login until the database auth is there!" );
    else
        error( No, "no such user" );
}
