#ifndef GPRUSAK_GEOM_H_
#define GPRUSAK_GEOM_H_

#include "gprusak/string.h"
#include "gprusak/log.h"
#include "gprusak/typeset.h"

namespace gprusak {

template<typename T> struct Pt {
  static_assert(TypeSet<int64_t,double>::contains<T>());
  T x,y;
  str show() const { return fmt("(%,%)",x,y); }
  INL auto operator<=>(const Pt &) const = default;

  INL friend Pt operator*(T f, Pt p){ return {f*p.x,f*p.y}; }
  INL friend Pt operator*(Pt p, T f){ return {p.x*f,p.y*f}; }
  INL friend Pt operator/(Pt p, T f){ return {p.x/f,p.y/f}; }
  INL friend T operator*(Pt a, Pt b){ return a.x*b.y-a.y*b.x; }
  INL friend Pt operator+(Pt a, Pt b){ return {a.x+b.x,a.y+b.y}; }
  INL friend Pt operator-(Pt a, Pt b){ return {a.x-b.x,a.y-b.y}; }
  INL friend Pt & operator+=(Pt &a, Pt b){ return a = a+b; }
  INL friend Pt & operator-=(Pt &a, Pt b){ return a = a-b; }
};

template<typename T> struct Range {
  T l,h;
  auto operator<=>(const Range &) const = default;
  str show() const { return fmt("[%,%]",l,h); }
  
  INL T size() const { return std::max<T>(0,h-l); }
  INL bool has(T v) const { return l<=v && v<=h; }
  INL T at(double f) const { return l+(h-l)*f; }
  INL Range inner(T border) const { return {l+border,h-border}; }
  INL Range operator+(T v) const { return {l+v,h+v}; }
  INL Range operator-(T v) const { return {l-v,h-v}; }
  INL Range operator*(T v) const { return {l*v,h*v}; }
  INL Range operator/(T v) const { return {l/v,h/v}; }
  INL Range operator&(const Range &b) const { return {std::max(l,b.l),std::min(h,b.h)}; }
  INL Range operator|(const Range &b) const { return {std::min(l,b.l),std::max(h,b.h)}; }
};

template<typename T> struct Rect {
  INL static Rect FromPoints(Pt<T> l, Pt<T> h){ return {.x={l.x,h.x},.y={l.y,h.y}}; }

  Range<T> x,y;
  INL int w() const { return x.size(); }
  INL int h() const { return y.size(); }
  INL Pt<T> low() const { return {x.l,y.l}; }
  INL Pt<T> high() const { return {x.h,y.h}; }
  INL Pt<T> size() const { return {x.size(),y.size()}; }
  auto operator<=>(const Rect &) const = default;

  INL bool has(Pt<T> p) const { return x.has(p.x) && y.has(p.y); }
  INL T area() const { return w()*h(); }
  str show() const { return fmt("%x%",x.show(),y.show()); }

  INL Rect inner(int border) const { return {x.inner(border),y.inner(border)}; }

  INL Rect operator+(Pt<T> a) const { return {x+a.x,y+a.y}; }
  INL Rect operator-(Pt<T> a) const { return {x-a.x,y-a.y}; }
  INL Rect operator/(T f) const { return {x/f,y/f}; }
  INL Rect operator*(T f) const { return {x*f,y*f}; }
  INL friend Rect operator&(const Rect &a, const Rect &b){ return {a.x&b.x,a.y&b.y}; }
};

}  // namespace gprusak

#endif  // GPRUSAK_GEOM_H_
