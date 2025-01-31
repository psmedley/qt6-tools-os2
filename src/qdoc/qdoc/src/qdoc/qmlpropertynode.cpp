// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "qmlpropertynode.h"

#include "classnode.h"
#include "propertynode.h"
#include "enumnode.h"

#include <utility>
#include "qdocdatabase.h"
#include "utilities.h"

QT_BEGIN_NAMESPACE

using namespace Qt::StringLiterals;

/*!
  Constructor for the QML property node.
 */
QmlPropertyNode::QmlPropertyNode(Aggregate *parent, const QString &name, QString type,
                                 bool attached)
    : Node(QmlProperty, parent, name),
      m_type(std::move(type)),
      m_attached(attached)
{
    if (m_type == "alias")
        m_isAlias = true;
    if (name.startsWith("__"))
        setStatus(Internal);
}

/*!
    Sets the data type of this property to \a dataType,
    preserving the list property modifier if one is set
    already.
*/
void QmlPropertyNode::setDataType(const QString &dataType)
{
    m_type = dataType;
    // Re-apply list modifier if needed
    if (auto is_list = m_isList; is_list == FlagValueTrue) {
        m_isList = FlagValueDefault;
        setIsList(true);
    }
}

/*!
  \fn bool QmlPropertyNode::isReadOnly() const

  Returns \c true if this QML property node is marked as a
  read-only property.
*/

/*!
    \fn const EnumNode *QmlPropertyNode::enumNode() const

    Returns the node representing the C++ enumeration associated
    with this property, or \nullptr.
*/

/*!
    Returns the prefix to use for documentated enumerators from
    the associated C++ enum for this property.
*/
const QString &QmlPropertyNode::enumPrefix() const
{
    return !m_enumNode.second.isEmpty() ?
            m_enumNode.second : parent()->name();
}

/*!
    Locates the node specified by \a path and sets it as the C++ enumeration
    associated with this property.

    \a registeredQmlName is used as the prefix in the generated enum value
    documentation.

    \note The target EnumNode is searched under the primary tree only.

    Returns \c true on success.
*/
bool QmlPropertyNode::setEnumNode(const QString &path, const QString &registeredQmlName)
{
    m_enumNode.first = static_cast<EnumNode*>(
        QDocDatabase::qdocDB()->primaryTree()->findNodeByNameAndType(path.split("::"), &Node::isEnumType)
    );
    m_enumNode.second = registeredQmlName;
    return m_enumNode.first != nullptr;
}

/*!
    Marks this property as a list if \a isList is \c true.
    The \c m_type member of a list property is wrapped with
    \c {list<>}.
*/
void QmlPropertyNode::setIsList(bool isList)
{
    if (m_isList != FlagValueDefault)
        return;

    if ((m_isList = toFlagValue(isList)))
        m_type = "list<%1>"_L1.arg(m_type);
}

/*!
  Returns \c true if this QML property or attached property is
  read-only. If the read-only status is not set explicitly
  using the \\readonly command, QDoc attempts to resolve it
  from the associated C++ class instantiated by the QML type
  that this property belongs to.

  \note Depending on how the QML type is implemented, this
  information may not be available to QDoc. If so, add a debug
  line but do not treat it as a warning.
 */
bool QmlPropertyNode::isReadOnly()
{
    if (m_readOnly != FlagValueDefault)
        return fromFlagValue(m_readOnly, false);

    // Find the parent QML type node
    auto *parent{this->parent()};
    while (parent && !(parent->isQmlType()))
        parent = parent->parent();

    bool readonly{false};
    if (auto qcn = static_cast<QmlTypeNode *>(parent); qcn && qcn->classNode()) {
        if (auto propertyNode = findCorrespondingCppProperty(); propertyNode)
            readonly = !propertyNode->isWritable();
        else
            qCDebug(lcQdoc).nospace()
                    << qPrintable(defLocation().toString())
                    << ": Automatic resolution of QML property attributes failed for "
                    << name()
                    << " (Q_PROPERTY not found in the C++ class hierarchy known to QDoc. "
                    << "Likely, the type is replaced with a private implementation.)";
    }
    markReadOnly(readonly);
    return readonly;
}

/*!
    Returns \c true if this QML property is marked with \required or the
    corresponding C++ property uses the REQUIRED keyword.
*/
bool QmlPropertyNode::isRequired()
{
    if (m_required != FlagValueDefault)
        return fromFlagValue(m_required, false);

    PropertyNode *pn = findCorrespondingCppProperty();
    return pn != nullptr && pn->isRequired();
}

/*!
  Returns a pointer this QML property's corresponding C++
  property, if it has one.
 */
PropertyNode *QmlPropertyNode::findCorrespondingCppProperty()
{
    PropertyNode *pn;
    Node *n = parent();
    while (n && !(n->isQmlType()))
        n = n->parent();
    if (n) {
        auto *qcn = static_cast<QmlTypeNode *>(n);
        ClassNode *cn = qcn->classNode();
        if (cn) {
            /*
              If there is a dot in the property name, first
              find the C++ property corresponding to the QML
              property group.
             */
            QStringList dotSplit = name().split(QChar('.'));
            pn = cn->findPropertyNode(dotSplit[0]);
            if (pn) {
                /*
                  Now find the C++ property corresponding to
                  the QML property in the QML property group,
                  <group>.<property>.
                 */
                if (dotSplit.size() > 1) {
                    QStringList path(extractClassName(pn->qualifiedDataType()));
                    Node *nn = QDocDatabase::qdocDB()->findClassNode(path);
                    if (nn) {
                        auto *cn = static_cast<ClassNode *>(nn);
                        PropertyNode *pn2 = cn->findPropertyNode(dotSplit[1]);
                        /*
                          If found, return the C++ property
                          corresponding to the QML property.
                          Otherwise, return the C++ property
                          corresponding to the QML property
                          group.
                         */
                        return (pn2 ? pn2 : pn);
                    }
                } else
                    return pn;
            }
        }
    }
    return nullptr;
}

QT_END_NAMESPACE
