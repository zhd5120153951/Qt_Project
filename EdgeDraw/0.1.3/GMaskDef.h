/****************************************************************************
**
** Qtitan Library by Developer Machines
** 
** Copyright (c) 2009-2015 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#ifndef GLDMASK_DEF_H
#define GLDMASK_DEF_H

#include <QtCore/QProcess>

#define QTN_DECLARE_PRIVATE(Class) friend class Class##Private; \
Class##Private *qtn_d_ptr; \
Class##Private& qtn_d() { return *qtn_d_ptr; } \
const Class##Private& qtn_d() const { return *qtn_d_ptr; }

#define QTN_DECLARE_PUBLIC(Class) \
friend class Class; \
Class *qtn_p_ptr; \
inline void setPublic(Class *ptr) { qtn_p_ptr = ptr; } \
Class& qtn_p() { return *qtn_p_ptr; } \
const Class& qtn_p() const { return *qtn_p_ptr; }
#define QTN_INIT_PRIVATE(Class) \
qtn_d_ptr = new Class##Private(); qtn_d_ptr->setPublic(this);
#define QTN_D(Class) Class##Private& d = qtn_d();
#define QTN_P(Class) Class& p = qtn_p();
#define QTN_FINI_PRIVATE() \
delete qtn_d_ptr; qtn_d_ptr = Q_NULL;

#define QTN_DECLARE_EX_PRIVATE(Class) friend class Class##Private; \
Class##Private& qtn_d() { return reinterpret_cast<Class##Private &>(*qtn_d_ptr); } \
const Class##Private& qtn_d() const { return reinterpret_cast<const Class##Private &>(*qtn_d_ptr); }

#define QTN_DECLARE_EX_PUBLIC(Class) \
friend class Class; \
Class& qtn_p() { return static_cast<Class &>(*qtn_p_ptr); } \
const Class& qtn_p() const { return static_cast<Class &>(*qtn_p_ptr); }

#endif //GLDMASK_DEF_H

