// Copyright Oryx Mail Systems GmbH. All enquiries to info@oryx.com, please.

#ifndef FETCH_H
#define FETCH_H

#include "command.h"


class Message;


class Fetch
    : public Command
{
public:
    Fetch( bool = false );

    void parse();
    void execute();

    void parseAttribute( bool alsoMacro );
    void parseBody();

    String dotLetters( uint, uint );

private:
    String fetchResponse( Message *, uint, uint );
    String flagList( Message *, uint );
    String internalDate( Message * );
    String envelope( Message * );
    String bodystructure( Message *, bool );

private:
    bool uid;
    class FetchData * d;
};


#endif
