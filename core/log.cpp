// Copyright Oryx Mail Systems GmbH. All enquiries to info@oryx.com, please.

#include "log.h"

#include "scope.h"
#include "logger.h"
#include "string.h"

// gettimeofday
#include <sys/time.h>
// localtime
#include <time.h>
// sprintf, fprintf
#include <stdio.h>


static bool disasters = false;
static String time();


void log( const String &m, Log::Severity s )
{
    Scope * cs = Scope::current();
    Log *l = 0;
    if ( cs )
        l = cs->log();
    if ( l )
        l->log( m, s );
}


void commit( Log::Severity s )
{
    Scope * cs = Scope::current();
    Log *l = 0;
    if ( cs )
        l = cs->log();
    if ( l )
        l->commit( s );
}


/*! \class Log log.h
    The Log class sends log messages to the Log server.

    A Log object accepts messages via log() and sends them to the log
    server. The log server can be instructed to commit() all messages of
    or above a certain priority, logged since the last such instruction,
    and discard the others.

    If a Log is destroyed (or the program dies), all pending messages
    are committed to disk by the log server.
*/

/*! Constructs an empty Log object with facility \a f. */

Log::Log( Facility f )
    : fc( f )
{
    Scope * cs = Scope::current();
    Log *l = 0;
    if ( cs )
        l = cs->log();
    if ( l )
        id = l->id + "/" + fn( l->children++ );
    else
        id = "1";
    children = 1;
}


/*! Changes this Log's facility to \a f. */

void Log::setFacility( Facility f )
{
    fc = f;
}


/*! Logs \a m using severity \a s. What happens to the message depends
    on the type of Logger used, and the log server configuration.
*/

void Log::log( const String &m, Severity s )
{
    Logger *l = Logger::global();
    if ( s == Disaster ) {
        String n = "Mailstore";
        if ( l )
            n = l->name();
        fprintf( stderr, "%s: %s\n", n.cstr(), m.simplified().cstr() );
    }

    if ( !l )
        return;

    String t( id );
    t.reserve( m.length() );
    t.append( " " );
    t.append( facility( fc ) );
    t.append( "/" );
    t.append( severity( s ) );
    t.append( " " );
    t.append( time() );
    t.append( " " );
    t.append( m.simplified() );
    t.append( "\r\n" );
    l->send( t );
}


/*! Requests the log server to commit all log statements with severity
    \a s or more to disk.
*/

void Log::commit( Severity s )
{
    Logger *l = Logger::global();
    if ( !l )
        return;

    String t( id );
    t.append( " commit " );
    t.append( facility( fc ) );
    t.append( "/" );
    t.append( severity( s ) );
    t.append( "\r\n" );
    l->send( t );
}


/*! Destroys a Log. Uncommitted messages are written to the log file. */

Log::~Log()
{
    commit( Debug );
}


/* This static function returns a nicely-formatted timestamp. */

static String time()
{
    struct timeval tv;
    struct timezone tz;
    if ( ::gettimeofday( &tv, &tz ) < 0 )
        return "";
    struct tm * t = localtime( (const time_t *)&tv.tv_sec );

    // yuck.
    char result[32];
    sprintf( result, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
             t->tm_year + 1900, t->tm_mon+1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec,
             (int)tv.tv_usec/1000 );
    return result;
}


/*! This static function returns a string describing \a s. */

const char *Log::severity( Severity s )
{
    const char *i = 0;

    switch ( s ) {
    case Log::Debug:
        i = "debug";
        break;
    case Log::Info:
        i = "info";
        break;
    case Log::Error:
        i = "error";
        break;
    case Log::Disaster:
        i = "disaster";
        break;
    }

    return i;
}


/*! This static function returns a string describing \a f. */

const char *Log::facility( Facility f )
{
    const char *i = 0;

    switch ( f ) {
    case Configuration:
        i = "configuration";
        break;
    case Database:
        i = "database";
        break;
    case Authentication:
        i = "authentication";
        break;
    case IMAP:
        i = "imap";
        break;
    case SMTP:
        i = "smtp";
        break;
    case Server:
        i = "server";
        break;
    case General:
        i = "general";
        break;
    }

    return i;
}


/*! Returns true if at least one disaster has been logged (on any Log
    object), and false if none have been.

    The disaster need not be committed - disastersYet() returns true as
    soon as log() has been called for a disastrous error.
*/

bool Log::disastersYet()
{
    return disasters;
}
