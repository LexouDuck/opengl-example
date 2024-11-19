#ifndef ELEMENT_H
#define ELEMENT_H

#include <string>
#include <vector>
#include <functional>
#include <algorithm>  // For std::remove
#include <dbg.hpp>
#include <Structs/Pos.hpp>
#include "./Event/Event.hpp"
#include "./Style/Style.hpp"

// Forward declaration of Element class

using namespace Dynamic;
using namespace Style;

class Element {

public:

    struct Rect {

        float x = 0; float y = 0;
        float width = 0; float height = 0;

        Rect() {}

        Rect(float x, float y, float width, float height) {
            this->x = x; this->y = y;
            this->width = width; this->height = height;
        }

        // Get center of rect as a position
        Pos center() {
            return Pos(x + width / 2, y + height / 2);
        }
    };

    // A group of elements
    struct Group {

        std::vector<Element*> members;

        // Add a child element
        void add(Element* child) {
            members.push_back(child);
        }

        // Remove a child element
        void remove(Element* child) {
            members.erase(std::remove(members.begin(), members.end(), child), members.end());
        }
    };

    struct ListenerGroup {

        using ListenerFunc = void (Element::*)(Event&);
        std::vector<std::function<void(Event&)>> listeners;
        ListenerFunc func;

        ListenerGroup(ListenerFunc f) : func(f) {}
    };

    Style::Style style;
    std::string name = "Element";
    std::vector<ListenerGroup> listenerGroups;

    Element* primeAncestor = nullptr;
    Element* parent = nullptr;

    void* globalStatePtr = nullptr;

    Rect rect;
    Rect innerRect;
    float scrollOffset = 0;
    
    int key = 0;
    int groupNum = 0;
    int lastClickId = -1;

    bool needsDraw = true;          // Initialized to true because all elements always need to be drawn at first
    bool shouldDebugRect = false;
    bool debugRect = false;
    bool isMouseOverTarget = false;

    // List of children elements
    //std::vector<Element*> children;

    Group children;

    // Constructor that optionally takes a parent
    Element(Element* parent = nullptr) {

        this->parent = parent;

        // Add self to parent if possible
        if (parent) {

            parent->children.add(this);

            this->primeAncestor = parent->primeAncestor;
            this->globalStatePtr = parent->globalStatePtr;
        }
    }

    // Destructor
    virtual ~Element() {

        // Ensure all children are deleted when this element is destroyed
        for (auto* child : children.members) {
            delete child;
        }

        children.members.clear();
    }

    // Register a listener for a specific function (used for lookup)
    void listen(ListenerGroup::ListenerFunc func, const std::function<void(Event&)>& listener) {

        // Check if the function already has a listener group
        auto it = std::find_if(listenerGroups.begin(), listenerGroups.end(),
            [func](const ListenerGroup& group) {
                return func == group.func;  // Compare function pointers (addresses)
            });

        // If found, add the listener to the group
        if (it != listenerGroups.end()) {
            it->listeners.push_back(listener);
        }

        // If not found, create a new group
        else {
            ListenerGroup newGroup(func);
            newGroup.listeners.push_back(listener);
            listenerGroups.push_back(newGroup);
        }
    }

    // "tell" function to notify listeners of a specific function
    void tell(ListenerGroup::ListenerFunc tellingFunc, Event& e) {

        // Search for the listener group that matches the telling function
        auto it = std::find_if(listenerGroups.begin(), listenerGroups.end(),
            [tellingFunc](const ListenerGroup& group) {
                return tellingFunc == group.func;  // Compare function pointers (addresses)
            });

        // If a matching listener group is found, notify all its listeners
        if (it != listenerGroups.end()) {
            for (auto& listener : it->listeners) {
                listener(e);
            }
        }
    }

    // Anything that is done before draw
    virtual void beforeDraw(Event& e) {
        
        for (auto* child : children.members) {
            child->beforeDraw(e);
        }
    }

    // Draw wrapper for element's draw
    void drawSelf(Event& e) {

        // If / else to avoid two if statements for scissoring
        if (style.overflow == Overflow::Hidden)  {this->setScissor(); }
    
        this->draw(e);

        // Draw all children
        for (auto* child : children.members) {
            child->drawSelf(e);
        }

        if (style.overflow == Overflow::Hidden) {
            this->endScissor();
        }

        this->needsDraw = false;
    }

    // Default draw method that can be used by derived classes without explicitly overriding
    virtual void draw(Event& e) {

        if (this->style.background.color) {
            this->drawFillRect(style.background.color, rect.x, rect.y, rect.width, rect.height);
        }
    }

    bool isDragTarget(Event& e) {
        return (e.mouse.lb || e.mouse.rb) && this->lastClickId == e.id;
    }

    // Wrapper functions
    void onRefresh(const std::function<void(Event&)>& listener) { this->listen(&Element::refresh, listener); }
    void onMouseDown(const std::function<void(Event&)>& listener) { this->listen(&Element::mouseDown, listener); }
    void onMouseUp(const std::function<void(Event&)>& listener) { this->listen(&Element::mouseUp, listener); }
    void onMouseMove(const std::function<void(Event&)>& listener) { this->listen(&Element::mouseMove, listener); }
    void onDrag(const std::function<void(Event&)>& listener) { this->listen(&Element::mouseDrag, listener); }
    void onMouseEnter(const std::function<void(Event&)>& listener) { this->listen(&Element::mouseEnter, listener); }
    void onMouseLeave(const std::function<void(Event&)>& listener) { this->listen(&Element::mouseLeave, listener); }
    void onMouseWheel(const std::function<void(Event&)>& listener) { this->listen(&Element::mouseWheel, listener); }

    // Overridable functions
    virtual void mouseDown(Event& e) { propagateMouseDown(e); }
    virtual void mouseUp(Event& e) { propagateMouseUp(e); }
    virtual void mouseMove(Event& e) { propagateMouseMove(e); }
    virtual void mouseDrag(Event& e) { propagateMouseDrag(e); }
    virtual void mouseWheel(Event& e) { propagateMouseWheel(e); }

    virtual void mouseEnter(Event& e) {

        if (this->shouldDebugRect) {
            this->debugRect = true;
            this->refresh(e);
        }

        this->propagateMouseEnter(e);
    }

    virtual void mouseLeave(Event& e) {

        if (this->shouldDebugRect) {
            this->debugRect = false;
            this->refresh(e);
        }

        this->propagateMouseLeave(e);
    }

    void propagateMouseEnter(Event& e) {

        tell(&Element::mouseEnter, e);
        if (!e.propagate) { return; }

        for (auto it = children.members.rbegin(); it != children.members.rend(); ++it) {

            Element* child = *it;

            bool containsEvent = child->contains(e.mouse.pos);
            bool isMouseOverTarget = child->isMouseOverTarget;

            if (containsEvent && !isMouseOverTarget) {
                child->isMouseOverTarget = true;
                child->mouseEnter(e);
            }

            if (!e.propagate) { return; }
        }
    }

    void propagateMouseLeave(Event& e) {

        e.subject = this;

        tell(&Element::mouseLeave, e);
        if (!e.propagate) { return; }

        for (auto it = children.members.rbegin(); it != children.members.rend(); ++it) {

            Element* child = *it;

            bool containsEvent = child->contains(e.mouse.pos);
            bool isMouseOverTarget = child->isMouseOverTarget;

            if (!containsEvent && isMouseOverTarget) {
                child->isMouseOverTarget = false;
                child->mouseLeave(e);
            }

            if (!e.propagate) { return; }
        }
    }

    void propagateMouseDown(Event& e) {

        dbg("%i", e.id);

        tell(&Element::mouseDown, e);
        if (!e.propagate) { return; }

        for (auto it = children.members.rbegin(); it != children.members.rend(); ++it) {

            Element* child = *it;

            // Check if the e position is within the child's rectangle
            if (child->contains(e.mouse.pos)) { // Use -> to access members
                child->lastClickId = e.id;
                child->mouseDown(e);
            }

            if (!e.propagate) {
                return;
            }
        }
    }

    void propagateMouseUp(Event& e) {

        // Stop if listener does not pass "continue" flag
        tell(&Element::mouseUp, e);
        if (!e.propagate) { return; }

        for (auto it = children.members.rbegin(); it != children.members.rend(); ++it) {

            Element* child = *it;

            if (child->contains(e.mouse.pos)) { child->mouseUp(e); }
            if (!e.propagate) { return; }
        }
    }

    // Propagate mouseMove among children and handle drag es for drag targets
    void propagateMouseMove(Event& e) {

        // Stop if listener does not pass "continue" flag
        tell(&Element::mouseMove, e);
        if (!e.propagate) { return; }
        
        for (auto it = children.members.rbegin(); it != children.members.rend(); ++it) {

            // Get child and do some checking
            Element* child = *it;
            bool isMouseOverTarget = child->isMouseOverTarget;
            bool containsEvent = child->contains(e.mouse.pos);

            // If the child is a drag target, trigger the onDrag e
            if ((e.mouse.lb || e.mouse.rb) && child->lastClickId == e.id) { child->mouseDrag(e); }

            // Detect if mouse is entering or leaving
            if (containsEvent && !isMouseOverTarget) { child->isMouseOverTarget = true; child->mouseEnter(e); }
            if (!containsEvent && isMouseOverTarget) { child->isMouseOverTarget = false; child->mouseLeave(e); }
            if (containsEvent) { child->mouseMove(e); }

            if (!e.propagate) { return; }
        }
    }

    void propagateMouseDrag(Event& e) {

        // Stop if listener does not pass "continue" flag
        tell(&Element::mouseDrag, e);
        if (!e.propagate) { return; }

        for (auto it = children.members.rbegin(); it != children.members.rend(); ++it) {

            Element* child = *it;

            if (child->lastClickId == e.id) { child->mouseDrag(e); }
            if (!e.propagate) { return; }
        }
    }

    void propagateMouseWheel(Event& e) {

        tell(&Element::mouseWheel, e);
        if (!e.propagate) { return; }

        for (auto it = children.members.rbegin(); it != children.members.rend(); ++it) {

            Element* child = *it;

            // Check if the e position is within the child's rectangle
            if (child->contains(e.mouse.pos)) {
                child->mouseWheel(e);
            }

            if (!e.propagate) {
                return;
            }
        }
    }

    /*  An important part of event handling is knowing
        whether a position lies within an element. This
        function calculates for a rect, but can be overridden
        to cover other kinds of shapes */
    virtual bool contains(Pos& pos) {

        Rect& rect = this->rect;
        float x = pos.x;
        float y = pos.y;

        if (x < rect.x) { return false; }
        if (y < rect.y) { return false; }
        if (x > rect.x + rect.width) { return false; }
        if (y > rect.y + rect.height) { return false; }

        return true;
    }

    /*  Helper function to allow collision logic
        to be handled by children instead of self */
    bool anyChildContains(Pos& pos) {

        for (auto* child: children.members) {
            if (child->contains(pos)) {
                return true;
            }
        }

        return false;
    }

    // Obtain the parent rect (can be overridden for top-level elements)
    virtual Rect getParentRect() {
        if (this->parent) { return this->parent->rect; }
        return Rect(0, 0, 0, 0);
    }

    /*  The first step of resolving the correct size for each element is to
        Take a look at the elements which have set or unalterable sizes, such
        as text and any other element which has a set dimention of some kind.
        Importantly, we must determine the smallest each element can be given
        it's children's sizes and how they are arranged */
    virtual Rect resolveStaticSize() {

        // Calculate the sizes of all children
        Rect thisSize = Rect(0, 0, 0, 0);

        for (auto* child: children.members) {

            Rect childSize = child->resolveStaticSize();

            // Get child margins
            float childMarginLeft = child->style.margin.left.resolve(child->rect.width);
            float childMarginRight = child->style.margin.right.resolve(child->rect.width);
            float childMarginTop = child->style.margin.top.resolve(child->rect.height);
            float childMarginBottom = child->style.margin.bottom.resolve(child->rect.height);
            
            // Get total margin in each direction
            float totalHorizontalMargin = childMarginLeft + childMarginRight;
            float totalVerticalMargin = childMarginTop + childMarginBottom;

            // If we arrange our children Arrangement::Horizontally
            if (this->style.arrange == Arrangement::Horizontal) {

                thisSize.width += childSize.width;
                thisSize.width += totalHorizontalMargin;

                if (childSize.height + totalVerticalMargin > thisSize.height) {
                    thisSize.height = childSize.height + totalVerticalMargin;
                }
            }

            // If we are arranging them Arrangement::Vertically
            if (this->style.arrange == Arrangement::Vertical) {

                thisSize.height += childSize.height;
                thisSize.height += totalVerticalMargin;
                
                if (childSize.width + totalHorizontalMargin > thisSize.width) {
                    thisSize.width = childSize.width + totalHorizontalMargin;
                }
            }
        }

        // Absolute values which are set override all else
        if (this->style.size.width && !this->style.size.width.setting) { thisSize.width = this->style.size.width.val; }
        if (this->style.size.height && !this->style.size.height.setting) { thisSize.height = this->style.size.height.val; }

        this->rect.width = thisSize.width;
        this->rect.height = thisSize.height;

        return thisSize;
    }

    /*  Resolve sizes which are defined as relative to another size
        This needs to occur *after* we have resolved absolute sizes */
    virtual Rect resolveRelativeSize() {

        // Calculate the sizes of all children
        Rect parentRect = this->getParentRect();

        if (style.size.width) { rect.width = style.size.width.resolve(parentRect.width); }
        if (style.size.height) { rect.height = style.size.height.resolve(parentRect.height); }

        if (style.size.maxWidth) { rect.width = std::min(rect.width, style.size.maxWidth.resolve(parentRect.width)); }
        if (style.size.minWidth) { rect.width = std::max(rect.width, style.size.minWidth.resolve(parentRect.width)); }

        if (style.size.maxHeight) { rect.height = std::min(rect.height, style.size.maxHeight.resolve(parentRect.height)); }
        if (style.size.minHeight) { rect.height = std::max(rect.height, style.size.minHeight.resolve(parentRect.height)); }

        // Call on all children
        for (auto* child: children.members) {
            child->resolveRelativeSize();
        }

        return this->rect;
    }

    // Resolve the x/y of the rect based on sizes or unset sizes
    // Called when sizes have been resolved
    virtual void arrangeChildren() {

        // Arrange default
        float currentX = this->rect.x;
        float currentY = this->rect.y;
        int currentGroupNum = 0;

        // Wrapping
        //--------------------------------------------------

        for (auto* child: children.members) {

            float cMarginLeft = child->style.margin.left.resolve(child->rect.width);
            float cMarginRight = child->style.margin.right.resolve(child->rect.width);
            float cMarginTop = child->style.margin.top.resolve(child->rect.height);
            float cMarginBottom = child->style.margin.bottom.resolve(child->rect.height);
            
            float tHorizontalMargin = cMarginLeft + cMarginRight;
            float tVerticalMargin = cMarginTop + cMarginBottom;

            bool wrap = this->style.overflow == Overflow::Wrap;

            // If we arrange our children Arrangement::Horizontally
            if (this->style.arrange == Arrangement::Horizontal) {

                // Wrap children
                if (wrap && currentX + child->rect.width + tHorizontalMargin > this->rect.x + this->rect.width) {
                    currentX = this->rect.x;
                    currentY += child->rect.height + cMarginBottom;
                    currentGroupNum += 1;
                }

                child->rect.x = currentX + cMarginLeft;
                child->rect.y = currentY + cMarginTop;
                currentX += child->rect.width + cMarginLeft + cMarginRight;
            }

            // If we are arranging them Arrangement::Vertically
            if (this->style.arrange == Arrangement::Vertical) {

                // Wrap children
                if (wrap && currentY + child->rect.height + tVerticalMargin > this->rect.y + this->rect.height) {
                    currentY = this->rect.y;
                    currentX += child->rect.width + cMarginRight;
                    currentGroupNum += 1;
                }

                child->rect.x = currentX + cMarginLeft;
                child->rect.y = currentY + cMarginTop;
                currentY += child->rect.height + cMarginTop + cMarginBottom;
            }

            child->groupNum = currentGroupNum;
        }

        // Allow child to override it's own position
        //--------------------------------------------------

        for (auto* child: children.members) {

            if (child->style.position.left) { child->rect.x = rect.x + child->style.position.left.resolve(rect.width); }
            if (child->style.position.right) { child->rect.x = rect.x + rect.width - child->rect.width - child->style.position.right.resolve(rect.width); }
            if (child->style.position.top) { child->rect.y = rect.y + child->style.position.top.resolve(rect.height); }
            if (child->style.position.bottom) { child->rect.y = rect.y + rect.height - child->rect.height - child->style.position.bottom.resolve(rect.height); }
        }

        // Call self on children
        for (auto* child: children.members) {
            child->arrangeChildren();
        }
    }

    virtual void centerChildren() {

        // Vector to hold groups based on group number
        std::vector<std::vector<Element*>> groups; // Replace Element with the actual type of your child

        // Initialize groups based on the maximum expected group number
        int maxGroupNum = 0;

        for (auto* child: children.members) {
            maxGroupNum = std::max(maxGroupNum, child->groupNum);
        }

        // Resize groups vector to accommodate group numbers
        groups.resize(maxGroupNum + 1); // +1 to accommodate zero indexing

        // Populate groups based on groupNum
        for (auto* child: children.members) {
            groups[child->groupNum].push_back(child);
        }

        // Center each group
        for (auto& group : groups) {

            if (group.empty()) continue; // Skip empty groups

            // Calculate the bounding box for the group, considering margins
            float minX = std::numeric_limits<float>::max();
            float minY = std::numeric_limits<float>::max();
            float maxX = std::numeric_limits<float>::lowest();
            float maxY = std::numeric_limits<float>::lowest();

            for (auto* child : group) {

                // Children with a set position should be ignored as they have taken charge of their own position
                if (child->style.position) { continue; }

                // Calculate min and max positions including margins
                minX = std::min(minX, child->rect.x - child->style.margin.left.resolve(child->rect.width)); // Include left margin
                minY = std::min(minY, child->rect.y - child->style.margin.top.resolve(child->rect.height));  // Include top margin
                maxX = std::max(maxX, child->rect.x + child->rect.width + child->style.margin.right.resolve(child->rect.width));  // Include right margin
                maxY = std::max(maxY, child->rect.y + child->rect.height + child->style.margin.bottom.resolve(child->rect.height)); // Include bottom margin
            }

            // Calculate bounding widths and heights relative to the current element's rect
            float groupWidth = maxX - minX;
            float groupHeight = maxY - minY;

            // Calculate the center of the bounding box of the group
            float groupCenterX = minX + (groupWidth / 2);
            float groupCenterY = minY + (groupHeight / 2);

            // Calculate the target center relative to this element's rect
            float parentCenterX = this->rect.x + (this->rect.width / 2);
            float parentCenterY = this->rect.y + (this->rect.height / 2);

            // Calculate the offsets needed to move the group's center to the target center
            float groupOffsetX = parentCenterX - groupCenterX;
            float groupOffsetY = parentCenterY - groupCenterY;

            // Apply alignment
            for (auto* child : group) {

                // Ignore children with set positions
                if (child->style.position.left.set ||
                    child->style.position.right.set ||
                    child->style.position.top.set ||
                    child->style.position.bottom.set) {
                        continue;
                }

                float childCenterX = child->rect.x + child->rect.width / 2;
                float childCenterY = child->rect.y + child->rect.height / 2;

                float childOffsetX = groupCenterX - childCenterX;
                float childOffsetY = groupCenterY - childCenterY;

                // Apply horizontal alignment
                //--------------------------------------------------

                if (this->style.horizontalAlign == Alignment::Start) {
                    child->rect.x += (this->rect.x - minX);
                }

                else if (this->style.horizontalAlign == Alignment::Center) {

                    child->rect.x += groupOffsetX;

                    if (this->style.arrange == Arrangement::Vertical) {
                        child->rect.x += childOffsetX;
                    }
                }
                
                else if (this->style.horizontalAlign == Alignment::End) {
                    child->rect.x += (this->rect.x + this->rect.width - maxX);
                }

                // Apply vertical alignment
                //--------------------------------------------------

                if (this->style.verticalAlign == Alignment::Start) {
                    child->rect.y += (this->rect.y - minY);
                }

                else if (this->style.verticalAlign == Alignment::Center) {

                    child->rect.y += groupOffsetY;

                    if (this->style.arrange == Arrangement::Horizontal) {
                        child->rect.y += childOffsetY;
                    }
                }
                
                else if (this->style.verticalAlign == Alignment::End) {
                    child->rect.y += (this->rect.y + this->rect.height - maxY); // Align to the bottom
                }
            }
        }

        // Allow children to override their own positions
        for (auto* child: children.members) {

            if (child->style.position.left.set) { child->rect.x = this->rect.x + child->style.position.left.resolve(rect.width); }
            if (child->style.position.right.set) { child->rect.x = this->rect.x + this->rect.width - child->rect.width - child->style.position.right.resolve(rect.width); }

            if (child->style.position.top.set) { child->rect.y = this->rect.y + child->style.position.top.resolve(rect.height); }
            if (child->style.position.bottom.set) { child->rect.y = this->rect.y + this->rect.height - child->rect.height - child->style.position.bottom.resolve(rect.height); }
        }

        // Call centerChildren on all children to allow them to adjust as needed
        for (auto* child: children.members) {
            child->centerChildren();
        }
    }

    // Calculate our inner rect
    virtual void calcInnerRect() {

        float minX = 999999999; float maxX = -999999999;
        float minY = 999999999; float maxY = -999999999;

        for (auto* child: children.members) {
            
            float childMinX = child->rect.x;
            float childMinY = child->rect.y;

            float childMaxX = child->rect.x + child->rect.width;
            float childMaxY = child->rect.y + child->rect.height;

            if (child->style.margin.left) { childMinX -= child->style.margin.left.resolve(child->rect.width); }
            if (child->style.margin.right) { childMaxX += child->style.margin.right.resolve(child->rect.width); }
            if (child->style.margin.top) { childMinY -= child->style.margin.top.resolve(child->rect.height); }
            if (child->style.margin.bottom) { childMaxY += child->style.margin.bottom.resolve(child->rect.height); }

            if (childMinX < minX) { minX = childMinX; }
            if (childMaxX > maxX) { maxX = childMaxX; }
            if (childMinY < minY) { minY = childMinY; }
            if (childMaxY > maxY) { maxY = childMaxY; }
        }

        this->innerRect.x = minX;
        this->innerRect.y = minY;
        this->innerRect.width = maxX - minX;
        this->innerRect.height = maxY - minY;

        // Calculate child inner rects
        for (auto* child: children.members) {
            child->calcInnerRect();
        }
    }

    // Calculate scroll if necessary
    virtual void calcScroll(float offset) {

        offset += this->scrollOffset;

        for (auto* child : children.members) {
            child->rect.y += offset;
        }

        for (auto* child : children.members) {
            child->calcScroll(offset);
        }
    }

    Rect getRect() {

        return Rect(0, 0, 0, 0);
    }

    void startScissor() {
        
        glEnable(GL_SCISSOR_TEST);

        glScissor(static_cast<GLint>(rect.x),
                static_cast<GLint>(primeAncestor->rect.height - (rect.y + rect.height)),
                static_cast<GLsizei>(rect.width),
                static_cast<GLsizei>(rect.height));
    }

    void setScissor() {
        glScissor(static_cast<GLint>(rect.x),
                static_cast<GLint>(primeAncestor->rect.height - (rect.y + rect.height)),
                static_cast<GLsizei>(rect.width),
                static_cast<GLsizei>(rect.height));
    }

    void endScissor() {
        glDisable(GL_SCISSOR_TEST);
    }

    virtual void refresh(Event& e) {
        this->needsDraw = true;
        if (!this->parent) { return; }
        this->parent->refresh(e);
    }

    // Drawing wrappers
    //--------------------------------------------------

    // Simple rect debugging function
    void dbgRect() {

        Style::Color color = Style::Color(1, 1, 0, 1);

        this->drawDashedRect(color, this->rect.x, this->rect.y, this->rect.width, this->rect.height, 1.0, 5.0);
    }

    // Draw a rectangle
    void drawRect(Color& color, float x, float y, float width, float height, float lineWidth = 1.0, float dashLength = 0) {

        // Don't draw no-color shapes
        if (!color.a) { return; }

        if (dashLength) {
            glEnable(GL_LINE_STIPPLE);
        }

        glLineWidth(lineWidth);
        glColor4f(color.r, color.g, color.b, color.a);

        glBegin(GL_LINE_LOOP);
            glVertex2f(x, y); glVertex2f(x + width, y);
            glVertex2f(x + width, y + height); glVertex2f(x, y + height);
        glEnd();

        glDisable(GL_LINE_STIPPLE);
    }

    void drawDashedRect(Color& color, float x, float y, float width, float height, 
            float lineWidth = 1.0, float dashLength = 5.0, float gapLength = 0.0) {
        
        // Don't draw no-color shapes
        if (!color.a) { return; }

        if (!gapLength) { gapLength = dashLength; }

        glLineWidth(lineWidth);
        
        // Draw each side of the rectangle with dashed lines
        drawDashedLine(color, x, y, x + width, y, dashLength, gapLength);          // Top
        drawDashedLine(color, x + width, y, x + width, y + height, dashLength, gapLength); // Right
        drawDashedLine(color, x + width, y + height, x, y + height, dashLength, gapLength); // Bottom
        drawDashedLine(color, x, y + height, x, y, dashLength, gapLength);      // Left
    }

    void drawDashedLine(const Color& color, float x1, float y1, float x2, float y2, float dashLength, float gapLength = 0) {

        if (!gapLength) {
            gapLength = dashLength;
        }

        glColor4f(color.r, color.g, color.b, color.a);

        float dx = x2 - x1; // Change in x
        float dy = y2 - y1; // Change in y
        float lineLength = sqrt(dx * dx + dy * dy); // Total line length

        // Normalize direction
        float unitX = dx / lineLength; 
        float unitY = dy / lineLength; 

        // Current position along the line
        float totalLength = 0.0f;

        // Start drawing dashes and gaps
        while (totalLength < lineLength) {
            // Draw a dash
            float dashEnd = totalLength + dashLength;
            if (dashEnd > lineLength) dashEnd = lineLength; // Adjust if the dash exceeds the line length

            // Calculate the end point of the dash
            float xDashEnd = x1 + unitX * dashEnd;
            float yDashEnd = y1 + unitY * dashEnd;

            glBegin(GL_LINES);
                glVertex2f(x1 + unitX * totalLength, y1 + unitY * totalLength); // Start of the dash
                glVertex2f(xDashEnd, yDashEnd); // End of the dash
            glEnd();

            // Update total length by the length of the dash
            totalLength += dashLength;

            // Skip the gap
            totalLength += gapLength; // Move total length forward by gap length
        }
    }

    // Draw a filled rectangle
    void drawFillRect(const Color& color, float x, float y, float width, float height) {

        // Don't draw no-color shapes
        if (!color.a) { return; }

        // Set the color
        glColor4f(color.r, color.g, color.b, color.a);

        // Start drawing filled rectangle
        glBegin(GL_POLYGON); // Draw a filled polygon (rectangle)
            glVertex2f(x, y); // Bottom-left corner
            glVertex2f(x + width, y); // Bottom-right corner
            glVertex2f(x + width, y + height); // Top-right corner
            glVertex2f(x, y + height); // Top-left corner
        glEnd();
    }

    // Draw a circle
    void drawCircle(const Color& color, float cx, float cy, float radius, float lineWidth = 1.0, int numSegments = 100) {

        // Don't draw no-color shapes
        if (!color.a) { return; }

        // Set line width
        glLineWidth(lineWidth);
        glColor4f(color.r, color.g, color.b, color.a);

        glBegin(GL_LINE_LOOP);

        float drawRadius = radius - lineWidth / 2.f;

        for (int i = 0; i < numSegments; ++i) {
            float theta = 2.0f * M_PI * (float(i) / float(numSegments));
            float x = drawRadius * cosf(theta); float y = drawRadius * sinf(theta);
            glVertex2f(x + cx, y + cy);
        }

        glEnd();
    }

    // Draw a filled circle using GL_POINTS and GL_POINT_SMOOTH
    void drawFillCircle(const Color& color, float cx, float cy, float radius) {
        
        // Enable point smoothing for round points
        glEnable(GL_POINT_SMOOTH);

        // Set the color
        glColor4f(color.r, color.g, color.b, color.a);

        // Set the point size based on the radius
        glPointSize(2 * radius);

        // Draw the point at the specified location
        glBegin(GL_POINTS);
        glVertex2f(cx, cy);
        glEnd();

        // Disable point smoothing to avoid affecting other drawing operations
        glDisable(GL_POINT_SMOOTH);
    }

    // Draw a line
    void drawLine(const Color& color, float x1, float y1, float x2, float y2, float lineWidth = 1.0) {

        glLineWidth(lineWidth);
        glColor4f(color.r, color.g, color.b, color.a);

        glBegin(GL_LINES);
            glVertex2f(x1, y1);
            glVertex2f(x2, y2);
        glEnd();
    }
};

#endif // ELEMENT_H