// Copyright Oryx Mail Systems GmbH. All enquiries to info@oryx.com, please.

#include "multipart.h"

#include "message.h"
#include "bodypart.h"
#include "mimefields.h"
#include "ustring.h"
#include "codec.h"


static const char * crlf = "\015\012";


/*! \class Multipart multipart.h
    This class represents the common characteristics of Messages and
    Bodyparts, namely that they have a header() and children().
*/

/*! Constructs an empty Multipart object.
*/

Multipart::Multipart()
    : h( 0 ), p( 0 ), parts( new List< Bodypart > )
{
}


/*! Returns a pointer to the Header for this Multipart object, or 0 if
    none has been set with setHeader().
*/

Header *Multipart::header() const
{
    return h;
}


/*! Sets the header of this Multipart object to \a hdr. */

void Multipart::setHeader( Header *hdr )
{
    h = hdr;
}


/*! Returns a pointer to the parent of this Multipart, or 0 if this is a
    top-level MIME object.
*/

Multipart *Multipart::parent() const
{
    return p;
}


/*! Sets the parent of this Multipart object to \a pt. */

void Multipart::setParent( Multipart *pt )
{
    p = pt;
}


/*! Returns a pointer to a list of Bodyparts belonging to this object.
    Will never return 0.
*/

List< Bodypart > *Multipart::children() const
{
    return parts;
}


/*! This function appends the text of a multipart MIME bodypart
    containing \a parts, with the Header \a h, to the string \a r.

    The details of this function are certain to change.
*/

void Multipart::appendMultipart( String & r, List< Bodypart > *parts,
                                 Header *h ) const
{
    ContentType * ct = h->contentType();
    String delim = ct->parameter( "boundary" );
    List<Bodypart>::Iterator it( parts->first() );
    r.append( "--" + delim + crlf );
    while ( it ) {
        Bodypart * bp = it;
        ++it;

        r.append( bp->header()->mimeFields() );
        r.append( crlf );
        appendAnyPart( r, bp, ct );

        r.append( String(crlf) + "--" + delim );
        if ( !it )
            r.append( "--" );
        r.append( crlf );
    }
}


/*! This function appends the text of the MIME bodypart \a bp with
    Content-type \a ct to the string \a r.

    The details of this function are certain to change.
*/

void Multipart::appendAnyPart( String &r, const Bodypart *bp,
                               ContentType *ct ) const
{
    ContentType *childct = bp->header()->contentType();

    if ( ( ct && ct->type() == "multipart" && ct->subtype() == "digest" &&
           !childct ) || ( childct && childct->type() == "message" ) )
    {
        // We only expect message/rfc822 here for now.
        r.append( bp->rfc822()->rfc822() );
    }

    else if ( !childct || childct->type().lower() == "text" ) {
        appendTextPart( r, bp, childct );
    }

    else if ( childct->type() == "multipart" ) {
        appendMultipart( r, bp->children(), bp->header() );
    }

    else {
        r.append( bp->data().e64( 72 ) );
    }
}


/*! This function appends the text of the MIME bodypart \a bp with
    Content-type \a ct to the string \a r.

    The details of this function are certain to change.
*/

void Multipart::appendTextPart( String & r, const Bodypart * bp,
                                ContentType * ct ) const
{
    Codec *c = 0;

    String::Encoding e = String::Binary;
    ContentTransferEncoding * cte
        = bp->header()->contentTransferEncoding();
    if ( cte )
        e = cte->encoding();

    if ( ct && !ct->parameter( "charset" ).isEmpty() )
        c = Codec::byName( ct->parameter( "charset" ) );
    if ( !c )
        c = Codec::byString( bp->text() );

    String body = c->fromUnicode( bp->text() );

    r.append( body.encode( e, 72 ) );
}
