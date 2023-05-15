/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef RECTPACKER_H
#define RECTPACKER_H

#include <string.h>

namespace rectpacker {

struct Rect
{
    Rect() : x(0), y(0), right(0), bottom(0) { }
    Rect(int rx, int ry, int rr, int rb) : x(rx), y(ry), right(rr), bottom(rb) { }

    inline int width() const { return right - x + 1; }
    inline int height() const { return bottom - y + 1; }

    int x, y;
    int right, bottom;
};

class RectPacker
{
public:
    RectPacker();
    RectPacker(int w, int h);
    ~RectPacker();

    bool isValid() const { return root != 0; }

    struct Node
    {
        Node(Node* parent = 0) { memset(this, 0, sizeof(Node)); nodes[2] = parent; }
        ~Node() {
            if (nodes[2]) {
                if (nodes[2]->nodes[0] == this)
                    nodes[2]->nodes[0] = 0;
                if (nodes[2]->nodes[1] == this)
                    nodes[2]->nodes[1] = 0;
            }
            delete nodes[0];
            delete nodes[1];
        }

        inline Node* parent() { return nodes[2]; }

        Node* nodes[3];
        Rect rect;

        bool leaf;
    };

    unsigned int serial() const { return ser; }

    void init(int w, int h);
    void resize(int w, int h);

    Node* insert(int w, int h);
    void destroy();

    void take(RectPacker& other);

    unsigned int size() const { return root ? (root->rect.width() * root->rect.height()) : 0; }
    unsigned int used() const;

    static void clear(Node* node);

private:
    static void resizeNode(Node* node, int nr, int nb, int dw, int dh);

    Node* insertSize(Node* node, int w, int h);

    Node* root;
    unsigned int ser;
};

} // namespace rectpacker

#endif
