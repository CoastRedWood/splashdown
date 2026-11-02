// TokenString: strtok over one string, without strtok's static state
// (8 bytes, usually on the caller's stack). Names follow its own methods.
#ifndef TOKENSTRING_TOKENSTRING_H
#define TOKENSTRING_TOKENSTRING_H

class TokenString {
public:
    TokenString(char *s);
    char *NextToken(const char *delims);

    char *m_token;      // +0x0, the token NextToken returned last
    char *m_next;       // +0x4, where the next token starts (0: no more)
};

#endif
