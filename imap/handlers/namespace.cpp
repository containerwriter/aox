// Copyright 2009 The Archiveopteryx Developers <info@aox.org>

#include "namespace.h"

#include "imap.h"
#include "user.h"
#include "mailbox.h"

/*! \class Namespace namespace.h
    Implements the NAMESPACE extension specified in RFC 2342.

    Archiveopteryx uses a single namespace, and this command informs
    the client about how this space is set up.

    Because of client confusion, we no longer tell anyone about
    /users/<name>. It is the same as "", but we don't tell the client
    that explicitly.
*/


void Namespace::execute()
{
    EString personal, other, shared;

    personal = "((\"\" \"/\"))";
    other    = "((\"/users/\" \"/\"))"; // XXX: should consult namespaces.
    shared   = "((\"/\" \"/\"))";

    respond( "NAMESPACE " + personal + " " + other + " " + shared );
    finish();
}
