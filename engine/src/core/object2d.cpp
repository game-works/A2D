//
// Created by selya on 04.11.2018.
//

#include <a2d/core/object2d.h>
#include <a2d/core/component.h>

namespace a2d {

Object2D::Object2D() : is_active(false), layer(0), parent(nullptr), scale(1), position(), rotation() {

}

bool Object2D::IsActive() {
    return is_active;
}

void Object2D::SetActive(bool active) {
    if (is_active == active) return;
    is_active = active;
    for (const auto &s : components) {
        for (const pComponent &c : s.second) {
            c->SetActive(is_active);
        }
    }
    for (const pObject2D &c : children) {
        c->SetActive(is_active);
    }
}

const Matrix4f &Object2D::GetTransformMatrix() const {
    return transform_matrix;
}

int Object2D::GetLayer() {
    return layer;
}

void Object2D::SetLayer(int layer) {
    if (layer == this->layer) return;
    this->LayerHierarchyAboutToChange();
    this->layer = layer;
    this->LayerHierarchyChanged();
}

pObject2D Object2D::GetParent() {
    return parent;
}

pObject2D Object2D::AddChild(pObject2D child) {
    if (child->parent != this) {
        if (child->parent != nullptr) child->parent->RemoveChild(child);
        child->parent = this;
        children.insert(child);
        child->SetActive(true);
    }
    return child;
}

pObject2D Object2D::RemoveChild(const pObject2D &child) {
    if (child->parent == this) {
        child->SetActive(false);
        child->parent = nullptr;
        children.erase(child);
    }
    return child;
}

void Object2D::LayerHierarchyAboutToChange() {
    for (const auto &s : components) {
        for (const pComponent &c : s.second) {
            c->LayerHierarchyAboutToChange();
        }
    }
    for (const pObject2D &c : children) {
        c->LayerHierarchyAboutToChange();
    }
}

void Object2D::LayerHierarchyChanged() {
    for (const auto &s : components) {
        for (const pComponent &c : s.second) {
            c->LayerHierarchyChanged();
        }
    }
    for (const pObject2D &c : children) {
        c->LayerHierarchyChanged();
    }
}

void Object2D::Update() {
    for (const auto &s : components) {
        for (const pComponent &c : s.second) {
            c->Update();
        }
    }
    for (const pObject2D &c : children) {
        c->Update();
    }
}

void Object2D::PostUpdate() {
    for (const auto &s : components) {
        for (const pComponent &c : s.second) {
            c->PostUpdate();
        }
    }
    for (const pObject2D &c : children) {
        c->PostUpdate();
    }
}

void Object2D::PreDraw(const a2d::Matrix4f &mat) {
    transform_matrix.Identity();
    transform_matrix.Translate(position.x, position.y, 0.0f);
    transform_matrix.Rotate(rotation, 0.0f, 0.0f, -1.0f);
    transform_matrix.Scale(scale.x, scale.y, 1.0f);

    transform_matrix = mat * transform_matrix;

    for (const auto &s : components) {
        for (const pComponent &c : s.second) {
            c->PreDraw();
        }
    }

    for (const pObject2D &c : children) {
        c->PreDraw(transform_matrix);
    }
}

void Object2D::PostDraw() {
    for (const auto &s : components) {
        for (const pComponent &c : s.second) {
            c->PostDraw();
        }
    }
    for (const pObject2D &c : children) {
        c->PostDraw();
    }
}

void Object2D::OnPause() {
    for (const auto &s : components) {
        for (const pComponent &c : s.second) {
            c->OnPause();
        }
    }
    for (const pObject2D &c : children) {
        c->OnPause();
    }
}

void Object2D::OnResume() {
    for (const auto &s : components) {
        for (const pComponent &c : s.second) {
            c->OnResume();
        }
    }
    for (const pObject2D &c : children) {
        c->OnResume();
    }
}

void Object2D::CleanTree() {
    for (const auto &s : components) {
        while (!s.second.empty()) RemoveComponent(*s.second.begin());
    }
    for (const pObject2D &c : children) {
        c->CleanTree();
    }
}

Object2D::~Object2D() {

}

} //namespace a2d
