<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c" %>
<%@ page session="false" %>
<html>
<head>
    <title>User</title>
    <link rel="stylesheet" type="text/css" href="<c:url value="/resources/style.css" />">
</head>
<body>
<div>
    <h1>user list</h1>
    <ul>
        <c:forEach items="${userList}" var="user">
            <li id="id_<c:out value="user.userId"/>">
                <div><c:out value="${user.userName}"/></div>
                <div>
                    <span>(<c:out value="${user.phoneNumber}"/>, <c:out value="${user.email}"/>)</span>
                </div>
            </li>
        </c:forEach>
    </ul>
</div>
</body>
</html>
