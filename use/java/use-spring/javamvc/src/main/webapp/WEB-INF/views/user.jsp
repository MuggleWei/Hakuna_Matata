<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c" %>
<%@ page session="false" %>
<html>
<head>
    <title>User</title>
</head>
<body>
<div>
    <div><c:out value="${user.userId}"/></div>
    <div><c:out value="${user.userName}"/></div>
    <div>
        <span>(<c:out value="${user.phoneNumber}"/>, <c:out value="${user.email}"/>)</span>
    </div>
</div>
</body>
</html>
