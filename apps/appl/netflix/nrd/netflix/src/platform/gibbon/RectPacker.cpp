/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "RectPacker.h"
#include <assert.h>

// #define PACK_DEBUG
#ifdef PACK_DEBUG
#include <stdio.h>
using namespace netflix;
#endif

namespace rectpacker {

RectPacker::RectPacker()
    : root(0), ser(0)
{
}

RectPacker::RectPacker(int w, int h)
    : root(0), ser(0)
{
    init(w, h);
}

RectPacker::~RectPacker()
{
    delete root;
}

static void used_helper(RectPacker::Node* node, unsigned int* used)
{
    if (node->leaf) {
        *used += (node->rect.width() * node->rect.height());
        return;
    }
    if (node->nodes[0])
        used_helper(node->nodes[0], used);
    if (node->nodes[1])
        used_helper(node->nodes[1], used);
}

unsigned int RectPacker::used() const
{
    if (!root)
        return 0;
    unsigned int u = 0;
    used_helper(root, &u);
    return u;
}

void RectPacker::init(int w, int h)
{
    ++ser;
    delete root;
    root = new Node;
    root->rect.right = w - 1;
    root->rect.bottom = h - 1;
}

void RectPacker::resize(int w, int h)
{
    const int diffw = root->rect.right - (w - 1);
    const int diffh = root->rect.bottom - (h - 1);
    assert(diffw > 0 && diffh > 0);
    resizeNode(root, w - 1, h - 1, diffw, diffh);
}

void RectPacker::resizeNode(Node* node, int nr, int nb, int dw, int dh)
{
    if (node->rect.right > nr)
        node->rect.right -= dw;
    assert(node->rect.right > 0);
    if (node->rect.bottom > nb)
        node->rect.bottom -= dh;
    assert(node->rect.bottom > 0);

    if (node->nodes[0])
        resizeNode(node->nodes[0], nr, nb, dw, dh);
    if (node->nodes[1])
        resizeNode(node->nodes[1], nr, nb, dw, dh);
}

void RectPacker::destroy()
{
    delete root;
    root = 0;
}

void RectPacker::clear(Node* node)
{
    delete node->nodes[0];
    node->nodes[0] = 0;
    delete node->nodes[1];
    node->nodes[1] = 0;
    node->leaf = false;
}

void RectPacker::take(RectPacker& other)
{
    ++ser;
    delete root;
    root = other.root;
    other.root = 0;
}

RectPacker::Node* RectPacker::insertSize(Node* node, int w, int h)
{
#ifdef PACK_DEBUG
    printf("walking down %d,%d+%dx%d\n",
           node->rect.x, node->rect.y,
           node->rect.width(), node->rect.height());
#endif
    if (node->nodes[0]) {
        if (Node* inserted = insertSize(node->nodes[0], w, h))
            return inserted;
        if (node->nodes[1])
            return insertSize(node->nodes[1], w, h);
    }
    if (node->leaf)
        return 0;
    if (node->rect.width() < w || node->rect.height() < h)
        return 0;
    if (w == node->rect.width() && h == node->rect.height()) {
        node->leaf = true;
        return node;
    }
    node->nodes[0] = new Node(node);
    node->nodes[1] = new Node(node);

    int dw = node->rect.width() - w;
    int dh = node->rect.height() - h;
    assert(dw >= 0 && dh >= 0);
    if (dw > dh) {
#ifdef PACK_DEBUG
        printf("left\n");
#endif
        node->nodes[0]->rect = Rect(node->rect.x, node->rect.y,
                                    node->rect.x + w - 1, node->rect.bottom);
        node->nodes[1]->rect = Rect(node->rect.x + w, node->rect.y,
                                    node->rect.right, node->rect.bottom);
    } else {
#ifdef PACK_DEBUG
        printf("right\n");
#endif
        node->nodes[0]->rect = Rect(node->rect.x, node->rect.y,
                                    node->rect.right, node->rect.y + h - 1);
        node->nodes[1]->rect = Rect(node->rect.x, node->rect.y + h,
                                    node->rect.right, node->rect.bottom);
    }
#ifdef PACK_DEBUG
    printf("created nodes %d,%d+%dx%d, %d,%d+%dx%d (wanting %dx%d)\n",
           node->nodes[0]->rect.x, node->nodes[0]->rect.y,
           node->nodes[0]->rect.width(), node->nodes[0]->rect.height(),
           node->nodes[1]->rect.x, node->nodes[1]->rect.y,
           node->nodes[1]->rect.width(), node->nodes[1]->rect.height(),
           w, h);
#endif
    return insertSize(node->nodes[0], w, h);
}

RectPacker::Node* RectPacker::insert(int w, int h)
{
    return insertSize(root, w, h);
}

} // namespace rectpacker
