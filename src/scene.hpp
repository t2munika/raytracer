#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"

class SceneNode {
public:
  SceneNode(const std::string& name);
  virtual ~SceneNode();

  const Matrix4x4& get_transform() const { return m_trans; }
  const Matrix4x4& get_inverse() const { return m_invtrans; }
  
  void set_transform(const Matrix4x4& m)
  {
    m_trans = m;
    m_invtrans = m.invert();
  }

  void set_transform(const Matrix4x4& m, const Matrix4x4& i)
  {
    m_trans = m;
    m_invtrans = i;
  }

  void add_child(SceneNode* child)
  {
    m_children.push_back(child);
  }

  void remove_child(SceneNode* child)
  {
    m_children.remove(child);
  }

  // Callbacks to be implemented.
  // These will be called from Lua.
  void rotate(char axis, double angle);
  void scale(const Vector3D& amount);
  void translate(const Vector3D& amount);
  virtual HitInfo intersects(Point3D origin, Vector3D dir) const;
  Colour get_colour(Point3D origin, Vector3D dir, const Colour& ambient, 
					const Colour& bg, const std::list<Light*>& lights,
					int num_glossy_rays,int limit = 3);

  // Returns true if and only if this node is a JointNode
  virtual bool is_joint() const;
  std::string m_name;
  
protected:
  
  // Useful for picking
  int m_id;

  // Transformations
  Matrix4x4 m_trans;
  Matrix4x4 m_invtrans;

  // Hierarchy
  typedef std::list<SceneNode*> ChildList;
  ChildList m_children;
};

class CSGNode : public SceneNode {
public:
  enum Type {
	UNION,
	INTERSECTION,
	DIFFERENCE
  };

  CSGNode(const std::string& name, CSGNode::Type type);
  virtual ~CSGNode();

  virtual HitInfo intersects(Point3D origin, Vector3D dir) const;
protected:
  Type m_type;
};

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name,
               Primitive* primitive);
  virtual ~GeometryNode();

  const Material* get_material() const;
  Material* get_material();

  void set_material(Material* material)
  {
    m_material = material;
  }

  virtual HitInfo intersects(Point3D origin, Vector3D dir) const;

protected:
  Material* m_material;
  Primitive* m_primitive;
};

#endif
