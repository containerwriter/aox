// Copyright Oryx Mail Systems GmbH. All enquiries to info@oryx.com, please.

#ifndef FETCHER_H
#define FETCHER_H

#include "event.h"
#include "list.h"


class Row;
class Query;
class Message;
class Mailbox;
class MessageSet;
class PreparedStatement;


class Fetcher
    : public EventHandler
{
public:
    Fetcher( Mailbox *, List<Message> *, EventHandler * );
    Fetcher( Message *, EventHandler * );

    enum Type {
        Flags,
        Annotations,
        Addresses,
        OtherHeader,
        Body,
        PartNumbers,
        Trivia
    };

    void addMessages( List<Message> * );
    void setSession( class Session * );

    void fetch( Type );
    bool fetching( Type ) const;

    void execute();

    bool done() const;

    void setTransaction( class Transaction * );

private:
    class FetcherData * d;

private:
    void start();
    void findMessages();
    void prepareBatch();
    void makeQueries();
    void waitForEnd();
    void submit( Query * );
    MessageSet * findUids();
};


#endif
