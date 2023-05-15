/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <GraphicsEngine.h>
#include <Screen.h>
#include "OpenGLContext.h"
#include "OpenGLState.h"
#include "OpenGLShaders.h"
#include <nrd/AppLog.h>
#include <gibbon/config.h>
#include <nrdbase/Base64.h>
#include <nrdbase/tr1.h>

using namespace netflix::gibbon;
using namespace netflix;

namespace netflix {
namespace gibbon {
class DrawVBO : public DisplayList::Command
{
public:
    class Event : public Application::Event
    {
    public:
        Event(OpenGLContext* c, GLuint v) : ctx(c), vbo(v) { }

        void eventFired()
        {
            //printf("deleted vbo %x\n", vbo);
            ctx->state()->deleteBuffer(GL_ARRAY_BUFFER, vbo);
        }
    private:
        OpenGLContext* ctx;
        GLuint vbo;
    };

    inline DrawVBO() : Command(Type_GLVBO), ctx(0), size(0), vbo(0) {}
    inline DrawVBO(OpenGLContext* _ctx, const Surface::SharedPtr& _surface, int _size,
                   const Color& _color, GLuint _vbo, const std::vector<ftcache::Glyph::SharedPtr>& _glyphs)
        : Command(Type_GLVBO), ctx(_ctx), surface(_surface), size(_size), color(_color),
          vbo(_vbo), glyphs(_glyphs)
    {
    }
    virtual ~DrawVBO()
    {
#if !defined(GIBBON_NO_THREADANIMATION)
        if(!Animation::isAnimationThread()) {
            shared_ptr<Event> event(new Event(ctx, vbo));
            Animation::postEvent(event);
            return;
        }
#endif
        //printf("deleted vbo %x\n", vbo);
        ctx->state()->deleteBuffer(GL_ARRAY_BUFFER, vbo);
    }
    virtual std::string describe() const {
        std::ostringstream s;
        s << "DrawVBO()";
        return s.str();
    }
    virtual void apply(GraphicsEngine *graphics, State *state) const {
        if (!surface)
            return;

        Rect viewport = graphics->targetRect();
        //printf("rendering vbo %x with viewport %s\n", vbo, viewport.toString().c_str());
        if (!gibbon_float_compare(graphics->mXScale, 1.0)) {
            viewport.x /= graphics->mXScale;
            viewport.width /= graphics->mXScale;
        }
        if (!gibbon_float_compare(graphics->mYScale, 1.0)) {
            viewport.y /= graphics->mYScale;
            viewport.height /= graphics->mYScale;
        }
        const float scale = graphics->mState.scale;

        Transform projection;
        projection.ortho(0, viewport.width, 0, viewport.height, -1, 1);
        if (graphics->isScreen())
            projection.translate(state->offsetX, viewport.height - state->offsetY, 0);
        else
            projection.translate(state->offsetX, state->offsetY, 0);

        if(!gibbon_float_compare(scale, 1.0)) {
            projection.translate(-1, 1, 0);
            projection.scale(scale, scale, 1);
            projection.translate(1, -1, 0);
        }

        //printf("rendering vbo %x\n", vbo);
        graphics->drawVBO(surface, color, projection, vbo, size);
    }

    virtual void write(DisplayList::Writer &writer) const
    {
        int bytes;
        OpenGLFloats& floats = ctx->floats();
        ctx->readVBO(vbo, floats.ptr(), 5, size, &bytes);

        const char *in = reinterpret_cast<char*>(floats.ptr());
        std::vector<char> encoded;
        encoded.resize(Base64::encode_reserve(bytes));
        const char *end = Base64::encode(in, in + bytes, &encoded[0]);

        writer << surface;
        writer << size;
        writer << color;
        writer << std::string(&encoded[0], end - &encoded[0]);
    }
    virtual void read(DisplayList::Reader &reader)
    {
        ctx = OpenGLContext::context();
        OpenGLFloats& floats = ctx->floats();

        reader >> surface;
        reader >> size;
        reader >> color;

        std::string encoded;
        reader >> encoded;
        char* out = reinterpret_cast<char*>(floats.ptr());
        Base64::decode(encoded.c_str(), encoded.c_str() + encoded.size(), out);
        vbo = ctx->createVBO(floats.ptr(), 5, size);
    }

    OpenGLContext* ctx;
    Surface::SharedPtr surface;
    int size;
    Color color;
    GLuint vbo;
    std::vector<ftcache::Glyph::SharedPtr> glyphs;
};

class GraphicsGLTextData : public GibbonFreetype::TextData
{
public:
    struct Data {
        std::vector<Rect> regularSrc, regularDst;
        std::vector<Rect> outlineSrc, outlineDst;
        std::vector<ftcache::Glyph::SharedPtr> regularGlyphs, outlineGlyphs;
        Surface::SharedPtr surface;
        bool hasRegularColor, hasOutlineColor;
        Color regularColor, outlineColor;
    };
    std::vector<Data> datas;
    OpenGLContext* glContext;
    const DisplayList::SharedPtr& displayList;

    GraphicsGLTextData(OpenGLContext* ctx, const DisplayList::SharedPtr& list) : glContext(ctx), displayList(list) { }
    virtual ~GraphicsGLTextData();

    virtual void clear() { datas.clear(); }

    virtual void addEntry(Type type, const ftcache::Glyph::SharedPtr& glyph, int tx, int ty, const Surface::SharedPtr& surface, const Color& color)
    {
        Data* d = datas.empty() ? 0 : &datas.back();
        if (datas.empty() || d->surface != surface
            || (type == GraphicsGLTextData::Regular && ((d->hasRegularColor && d->regularColor != color) || d->regularGlyphs.size() >= 1024))
            || (type == GraphicsGLTextData::Outline && d->hasOutlineColor && d->outlineColor != color)) {
            Data nd;
            nd.surface = surface;
            if (type == GraphicsGLTextData::Regular) {
                nd.hasOutlineColor = false;
                nd.hasRegularColor = true;
                nd.regularColor = color;
            } else {
                nd.hasRegularColor = false;
                nd.hasOutlineColor = true;
                nd.outlineColor = color;
            }
            datas.push_back(nd);
            d = &datas.back();
        } else if (type == GraphicsGLTextData::Regular && !d->hasRegularColor) {
            d->hasRegularColor = true;
            d->regularColor = color;
        } else if (type == GraphicsGLTextData::Outline && !d->hasOutlineColor) {
            d->hasOutlineColor = true;
            d->outlineColor = color;
        }

        const rectpacker::Rect& adjusted = GibbonFreetype::adjustRect(glyph->glyphNode->rect);

        const Rect srcRect(adjusted.x, adjusted.y, (adjusted.right - adjusted.x) + 1, (adjusted.bottom - adjusted.y) + 1);
        const Rect dstRect(tx, ty, srcRect.width, srcRect.height);
        if(type == GibbonFreetype::TextData::Regular) {
            d->regularSrc.push_back(srcRect);
            d->regularDst.push_back(dstRect);
            d->regularGlyphs.push_back(glyph);
        } else {
            d->outlineSrc.push_back(srcRect);
            d->outlineDst.push_back(dstRect);
            d->outlineGlyphs.push_back(glyph);
        }
    }
};

GraphicsGLTextData::~GraphicsGLTextData()
{
    OpenGLFloats& floats = glContext->floats();

    const size_t dataSize = datas.size();
    for (size_t dataIdx = 0; dataIdx < dataSize; ++dataIdx) {
        const GraphicsGLTextData::Data& d = datas[dataIdx];

        const Surface::SharedPtr& surface = d.surface;
        assert(surface && surface->isValid());

        const GLfloat cw = surface->getWidth();
        const GLfloat ch = surface->getHeight();

        struct {
            const std::vector<Rect>& src;
            const std::vector<Rect>& dst;
            const std::vector<ftcache::Glyph::SharedPtr>& glp;
            const Color& color;
        } rects[] =
              { { d.outlineSrc, d.outlineDst, d.outlineGlyphs, d.outlineColor },
                { d.regularSrc, d.regularDst, d.regularGlyphs, d.regularColor } };
        for (int i = 0; i < 2; ++i) {
            const std::vector<Rect>& src = rects[i].src;
            const std::vector<Rect>& dst = rects[i].dst;
            const std::vector<ftcache::Glyph::SharedPtr>& glp = rects[i].glp;
            if (src.empty())
                continue;
            assert(src.size() == dst.size());
            assert(src.size() == glp.size());

            const size_t sz = src.size();
            floats.reserve(sz * 30);
            GLfloat* base = floats.take(0);

            for (size_t j = 0; j < sz; ++j) {
                const GLfloat tx = dst[j].x;
                const GLfloat ty = dst[j].y;
                const GLfloat sx1 = src[j].x;
                const GLfloat sy1 = src[j].y;
                const GLfloat sx2 = src[j].right() - 1;
                const GLfloat sy2 = src[j].bottom() - 1;

                GLfloat x0, y0, x1, y1;
                x0 = tx;
                y0 = ty;
                x1 = tx + ((sx2 - sx1) + 1);
                y1 = ty + ((sy2 - sy1) + 1);

                const GLfloat s0 = sx1 / cw;
                const GLfloat t0 = sy1 / ch;
                const GLfloat s1 = (sx2 + 1) / cw;
                const GLfloat t1 = (sy2 + 1) / ch;

                GLfloat* v = floats.take(30);

                *(v++) = x0;
                *(v++) = y0;
                *(v++) = 0;
                *(v++) = s0;
                *(v++) = t0;

                *(v++) = x0;
                *(v++) = y1;
                *(v++) = 0;
                *(v++) = s0;
                *(v++) = t1;

                *(v++) = x1;
                *(v++) = y1;
                *(v++) = 0;
                *(v++) = s1;
                *(v++) = t1;

                *(v++) = x0;
                *(v++) = y0;
                *(v++) = 0;
                *(v++) = s0;
                *(v++) = t0;

                *(v++) = x1;
                *(v++) = y1;
                *(v++) = 0;
                *(v++) = s1;
                *(v++) = t1;

                *(v++) = x1;
                *(v++) = y0;
                *(v++) = 0;
                *(v++) = s1;
                *(v++) = t0;
            }

            floats.reset();

            const GLuint vbo = glContext->createVBO(base, 5, sz);
            displayList->appendCommand(new DrawVBO(glContext, surface, sz, rects[i].color, vbo, glp));
        }
    }
    GL_CHECK;
}
}}

void GraphicsEngine::begin_sys()
{
    mGLContext = OpenGLContext::context();
    mGLTextData = 0;
    if(mSurface)
        mSurface->createTexture();
    if (mSurface && !(mSurface->getFlags() & Surface::Flags_Screen)) {
        mTargetRect = mSurface->getRect();
        mXScale = mYScale = 1.;
        mFbo.reset(new OpenGLFBO(mSurface));
    } else {
        const Rect rect = mDisplayList ? mDisplayList->getRect() : mSurface->getRect();
        mTargetRect = GibbonApplication::instance()->getScreen()->getRect();
        if (mDisplayList) {
            mXScale = mYScale = 1.;
        } else {
            mXScale = float(rect.width) / mTargetRect.width;
            mYScale = float(rect.height) / mTargetRect.height;
            mTargetRect = rect;
        }
    }
    assert(!mFbo || mFbo->isValid());

    mGLContext->pushEngine(this);
}

void GraphicsEngine::end_sys()
{
    assert(!mGLTextData);
    mFbo.reset();
    mGLContext->popEngine();
    mGLContext = 0;
}

static inline void makeTextureTransform(const Rect& surfaceRect, const Rect& rect, float textureTransform[4])
{
    if (rect != surfaceRect) {
        textureTransform[0] = static_cast<float>(rect.x) / surfaceRect.width;
        textureTransform[1] = static_cast<float>(rect.y) / surfaceRect.height;
        textureTransform[2] = static_cast<float>(rect.width) / surfaceRect.width;
        textureTransform[3] = static_cast<float>(rect.height) / surfaceRect.height;
    } else {
        static GLfloat noTextureTransform[4] = { 0, 0, 1, 1 };
        memcpy(textureTransform, noTextureTransform, sizeof(noTextureTransform));
    }
}

Rect GraphicsEngine::makeScissor(const Rect& rect)
{
    const Coordinate y = isScreen() ? mGLContext->screenHeight() - (rect.y * mYScale) - (rect.height * mYScale) : rect.y;
    return Rect(mXScale * rect.x, y, ceilf(mXScale * rect.width), ceilf(mXScale * rect.height));
}

Transform GraphicsEngine::makeTransform(const Rect& dstRect, const Rect& srcRect)
{
    Transform modelViewMatrix;

    const Size sz = mTargetRect.size();

    Rect src = srcRect;
    if (src.empty())
        src = Rect(0, 0, sz.width, sz.height);

    const float tx = ((dstRect.x + dstRect.width / 2.0f) - (sz.width / 2.0f)) / (sz.width / 2.0f);
    const float ty = ((sz.height / 2.0f) - (dstRect.y + dstRect.height / 2.0f)) / (sz.height / 2.0f);

    modelViewMatrix.translate(tx, ty, 0);

    const float sx = (float(dstRect.width) / src.width) * (float(src.width) / sz.width);
    const float sy = (float(dstRect.height) / src.height) * (float(src.height) / sz.height);

    modelViewMatrix.scale(sx, sy, 1);

    return modelViewMatrix;
}

void GraphicsEngine::drawVBO(const Surface::SharedPtr& surface, const Color& color, const Transform& modelview, GLuint vbo, int size)
{
    GL_CHECK;
    const shared_ptr<TextShader>& textShader = mGLContext->textShader();
    OpenGLShader::Scope<TextShader> scope(textShader);

    float c[4];
    color.normalized(c);
    c[3] *= mState.opacity;
    if (c[3]<1)
    {
        c[0] *= c[3];
        c[1] *= c[3];
        c[2] *= c[3];
    }

    Transform projectionMatrix;
    if (isScreen()) {
        projectionMatrix.scale(1, -1, 1);
    }
    glUniformMatrix4fv(textShader->variable(TextShader::ProjMatrix), 1, GL_FALSE, (GLfloat*)projectionMatrix.matrix());
    glUniform4fv(textShader->variable(TextShader::Color), 1, c);
    glUniformMatrix4fv(textShader->variable(TextShader::MVMatrix), 1, GL_FALSE, (GLfloat*)modelview.matrix());

    OpenGLState* glstate = mGLContext->state();
    OpenGLState::Scope stateScope(glstate);
    stateScope.enableCap(GL_SCISSOR_TEST, mState.hasClip);
    stateScope.enableCap(GL_BLEND);
    glstate->blendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glstate->bindBuffer(GL_ARRAY_BUFFER, vbo);

    GLchar* ptr = 0;
    stateScope.enableVAA(textShader->variable(TextShader::Vertex));
    glVertexAttribPointer(textShader->variable(TextShader::Vertex), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), ptr);

    ptr += 3 * sizeof(GLfloat);
    stateScope.enableVAA(textShader->variable(TextShader::TexCoord));
    glVertexAttribPointer(textShader->variable(TextShader::TexCoord), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), ptr);

    stateScope.flush();

    glstate->bindTexture(surface->getTexture());

    glDrawArrays(GL_TRIANGLES, 0, size * 6);

    GL_CHECK;
}

void GraphicsEngine::setScale_sys(float /*scale*/)
{
}

void GraphicsEngine::setOpacity_sys(float /*opacity*/)
{
}

void GraphicsEngine::setSmoothScale_sys(bool /*smooth*/)
{
}

void GraphicsEngine::setClip_sys(const Rect *r)
{
    GL_CHECK;
    const Rect scissor = makeScissor(r ? *r : mTargetRect);
    mGLContext->state()->scissor(scissor.x, scissor.y, scissor.width, scissor.height);
    GL_CHECK;
}

void GraphicsEngine::setCompositionMode_sys(GraphicsEngine::CompositionMode /*mode*/)
{
}

void GraphicsEngine::blit_internal(GLuint texture, const Transform& modelView, const GLfloat texTransform[4],
                                   bool blend, float opacity, bool smooth)
{
    GL_CHECK;
    const shared_ptr<BlitShader>& blitShader = mGLContext->blitShader();
    OpenGLShader::Scope<BlitShader> scope(blitShader);
    glUniformMatrix4fv(blitShader->variable(BlitShader::MVMatrix), 1, GL_FALSE, modelView.matrix());

    Transform projection;
    if (!isScreen()) {
        projection.scale(1, -1, 1);
    }

    projection.translate(-1, 1, 0);
    projection.scale(mXScale, mYScale, 1);
    projection.translate(1, -1, 0);

    glUniformMatrix4fv(blitShader->variable(BlitShader::ProjMatrix), 1, GL_FALSE, (GLfloat*)projection.matrix());
    glUniform4fv(blitShader->variable(BlitShader::TextureTransform), 1, texTransform);
    glUniform1f(blitShader->variable(BlitShader::Opacity), opacity);

    const GLuint pos = blitShader->variable(BlitShader::Position);
    const GLuint tex = blitShader->variable(BlitShader::TexCoord);

    OpenGLState* glstate = mGLContext->state();
    OpenGLState::Scope stateScope(glstate);
    stateScope.enableCap(GL_SCISSOR_TEST, mState.hasClip);
    stateScope.enableVAA(pos);
    stateScope.enableVAA(tex);
    if (blend) {
        glstate->blendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        stateScope.enableCap(GL_BLEND);
    }

    blitShader->bindVertexBuffer();
    glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    blitShader->bindTextureBuffer();
    glVertexAttribPointer(tex, 2, GL_UNSIGNED_BYTE, GL_FALSE, 2 * sizeof(GLubyte), 0);

    stateScope.flush();

    glstate->bindTexture(texture);
    glstate->setSmoothScale(smooth);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

    GL_CHECK;
}

void GraphicsEngine::blit(GLuint tex, const Transform& modelView, const GLfloat texTransform[4])
{
    blit_internal(tex, modelView, texTransform, true, 1., false);
}

void GraphicsEngine::blit_sys(const Surface::SharedPtr &srcSurface, const Rect &srcRect, const Rect &dstRect, const Color *)
{
    const bool srcAlpha = srcSurface->hasAlpha();
    const bool srcOpacity = !gibbon_float_compare(mState.opacity, 1.0);
    const bool blend = (getCompositionMode() == CompositionMode_SourceOver && (srcAlpha || srcOpacity));
    Rect blitSrcRect = srcRect;
    if (srcSurface->mMaster) {
        blitSrcRect.x += srcSurface->getRect().x;
        blitSrcRect.y += srcSurface->getRect().y;
    }
    Transform transform = makeTransform(dstRect, blitSrcRect);
    if (!gibbon_float_compare(mState.scale, 1.0)) {
        transform.translate(-1, 1, 0);
        transform.scale(mState.scale, mState.scale, 1);
        transform.translate(1, -1, 0);
    }
    GLfloat texTransform[4];
    makeTextureTransform(srcSurface->getMasterRect(), blitSrcRect, texTransform);

    GLuint texture = srcSurface->getTexture();
    blit_internal(texture, transform, texTransform, blend, mState.opacity, mState.smoothScale);
}

void GraphicsEngine::drawText_sys(hb_buffer_t* text, int start, int end, const Rect& r, const Rect& dstRect, const Style::SharedPtr &style)
{
    drawText_internal(text, start, end, r, dstRect, style);
}

void GraphicsEngine::initCommands_sys()
{
    DisplayList::registerFactory<DrawVBO>(DisplayList::Command::Type_GLVBO);
}

bool GraphicsEngine::startText_sys()
{
    assert(!mGLTextData);
    mGLTextData = new GraphicsGLTextData(mGLContext, mDisplayList);
    return true;
}

bool GraphicsEngine::endText_sys()
{
    assert(mGLTextData);
    delete mGLTextData;
    mGLTextData = 0;
    return true;
}

void GraphicsEngine::drawText_dl(hb_buffer_t *text, int start, int end, const Rect &rect, const Rect &dstRect, const Style::SharedPtr &style)
{
    GL_CHECK;
    if(GibbonConfiguration::screenRenderDisplayList()) {
        drawText_dl_internal(text, start, end, rect, dstRect, style);
        return;
    }

    GibbonFreetype::SharedPtr freetype = style->getFont()->getFreetype();
    bool del = mGLTextData == 0;
    if (del)
        mGLTextData = new GraphicsGLTextData(mGLContext, mDisplayList);
    freetype->prepareText(text, start, end, rect, style, mGLTextData);
    if (del) {
        delete mGLTextData;
        mGLTextData = 0;
    }
}

void GraphicsEngine::clear_sys(const Rect &rect)
{
    GL_CHECK;
    if (isScreen()
        && Rect(0, 0, mGLContext->screenWidth(), mGLContext->screenHeight()) == rect
        && mGLContext->renderVideo())
        return;

    Rect clip = rect.scaled(mState.scale, mState.scale);
    if (mState.hasClip)
        clip.intersect(mState.clip);
    else
        clip.intersect(mTargetRect);
    clip = makeScissor(clip);
    OpenGLState* glstate = mGLContext->state();
    glstate->scissor(clip.x, clip.y, clip.width, clip.height);
    glstate->enableCap(GL_SCISSOR_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    if (mState.hasClip) {
        clip = makeScissor(mState.clip);
        glstate->scissor(clip.x, clip.y, clip.width, clip.height);
    } else {
        glstate->disableCap(GL_SCISSOR_TEST);
    }
    GL_CHECK;
}

void GraphicsEngine::drawRect(RectMode mode, const Rect &rect, const Color &color)
{
    GL_CHECK;
    shared_ptr<FillShader> shader = (mode == RectFill) ? mGLContext->fillShader() : static_pointer_cast<FillShader>(mGLContext->lineShader());

    OpenGLShader::Scope<FillShader> scope(shader);

    float c[4];
    color.normalized(c);
    if (getCompositionMode() != CompositionMode_Source && mState.opacity < 1) {
        c[3] *= mState.opacity;
    }
    const bool hasAlpha = c[3] < 1;
    if (hasAlpha) {
        c[0] *= c[3];
        c[1] *= c[3];
        c[2] *= c[3];
    }
    glUniform4fv(shader->variable(FillShader::Color), 1, c);

    Transform transform = makeTransform(rect), projection;
    if(!gibbon_float_compare(mState.scale, 1.0)) {
        transform.translate(-1, 1, 0);
        transform.scale(mState.scale, mState.scale, 1);
        transform.translate(1, -1, 0);
    }
    glUniformMatrix4fv(shader->variable(FillShader::MVMatrix), 1, GL_FALSE, (GLfloat*)transform.matrix());
    if (!isScreen()) {
        projection.scale(1, -1, 1);
    }
    projection.translate(-1, 1, 0);
    projection.scale(mXScale, mYScale, 1);
    projection.translate(1, -1, 0);
    glUniformMatrix4fv(shader->variable(FillShader::ProjMatrix), 1, GL_FALSE, (GLfloat*)projection.matrix());

    const GLuint pos = shader->variable(FillShader::Position);
    OpenGLState* glstate = mGLContext->state();
    OpenGLState::Scope stateScope(glstate);
    if (hasAlpha) {
        glstate->blendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        stateScope.enableCap(GL_BLEND);
    }

    shader->bindVertexBuffer();
    glVertexAttribPointer(shader->variable(FillShader::Position), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    stateScope.enableCap(GL_SCISSOR_TEST, mState.hasClip);
    stateScope.enableVAA(pos);
    stateScope.flush();
    if (mode == RectFill) {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
    } else {
        glDrawElements(GL_LINES, 8, GL_UNSIGNED_BYTE, 0);
    }

    GL_CHECK;
}

void GraphicsEngine::fill_sys(const Rect &rect, const Color &color)
{
    drawRect(RectFill, rect, color);
}

void GraphicsEngine::drawRect_sys(const Rect &rect, const Color &color)
{
    drawRect(RectLine, rect, color);
}

void GraphicsEngine::batchBlit_sys(const Surface::SharedPtr& surface, const std::vector<Rect>& src,
                                   const std::vector<Rect>& dst, const Color *color)
{
    GL_CHECK;
    assert(surface && surface->isValid());
    assert(color);

    OpenGLFloats& floats = mGLContext->floats();

    const GLfloat cw = surface->getWidth();
    const GLfloat ch = surface->getHeight();

    if (src.empty())
        return;
    assert(src.size() == dst.size());

    const size_t sz = src.size();
    floats.reserve(sz * 30);
    GLfloat* base = floats.take(0);

    for (size_t j = 0; j < sz; ++j) {
        const GLfloat tx = dst[j].x;
        const GLfloat ty = dst[j].y;
        const GLfloat sx1 = src[j].x;
        const GLfloat sy1 = src[j].y;
        const GLfloat sx2 = src[j].right() - 1;
        const GLfloat sy2 = src[j].bottom() - 1;

        GLfloat x0, y0, x1, y1;
        x0 = tx;
        y0 = ty;
        x1 = tx + ((sx2 - sx1) + 1);
        y1 = ty + ((sy2 - sy1) + 1);

        const GLfloat s0 = sx1 / cw;
        const GLfloat t0 = sy1 / ch;
        const GLfloat s1 = (sx2 + 1) / cw;
        const GLfloat t1 = (sy2 + 1) / ch;

        GLfloat* v = floats.take(30);

        *(v++) = x0;
        *(v++) = y0;
        *(v++) = 0;
        *(v++) = s0;
        *(v++) = t0;

        *(v++) = x0;
        *(v++) = y1;
        *(v++) = 0;
        *(v++) = s0;
        *(v++) = t1;

        *(v++) = x1;
        *(v++) = y1;
        *(v++) = 0;
        *(v++) = s1;
        *(v++) = t1;

        *(v++) = x0;
        *(v++) = y0;
        *(v++) = 0;
        *(v++) = s0;
        *(v++) = t0;

        *(v++) = x1;
        *(v++) = y1;
        *(v++) = 0;
        *(v++) = s1;
        *(v++) = t1;

        *(v++) = x1;
        *(v++) = y0;
        *(v++) = 0;
        *(v++) = s1;
        *(v++) = t0;
    }

    floats.reset();

    const GLuint vbo = mGLContext->createVBO(base, 5, sz);

    Rect viewport = mTargetRect;
    //printf("rendering vbo %x with viewport %s\n", vbo, viewport.toString().c_str());
    if (!gibbon_float_compare(mXScale, 1.0)) {
        viewport.x /= mXScale;
        viewport.width /= mXScale;
    }
    if (!gibbon_float_compare(mYScale, 1.0)) {
        viewport.y /= mYScale;
        viewport.height /= mYScale;
    }
    const float scale = mState.scale;

    Transform projection;
    projection.ortho(0, viewport.width, 0, viewport.height, -1, 1);
    if (isScreen())
        projection.translate(0, viewport.height, 0);

    if(!gibbon_float_compare(scale, 1.0)) {
        projection.translate(-1, 1, 0);
        projection.scale(scale, scale, 1);
        projection.translate(1, -1, 0);
    }

    drawVBO(surface, *color, projection, vbo, sz);
    mGLContext->state()->deleteBuffer(GL_ARRAY_BUFFER, vbo);
    GL_CHECK;
}

void GraphicsEngine::flush_sys()
{
    glFinish();
}

void GraphicsEngine::freeSurfaces()
{
    if(OpenGLContext::hasContext()) {
        GL_CHECK;
        if (!GibbonConfiguration::uploadInDecodeThreads())
            OpenGLContext::deleteTextures();
        GL_CHECK;
    }
}
