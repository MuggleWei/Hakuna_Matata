<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c" %>
<%@ page session="false" %>
<html>
<head>
    <title>Home</title>
    <link rel="stylesheet"
          type="text/css"
          href="<c:url value="/resources/style.css" />" />
</head>
<body>
    <h1>Welcome</h1>
    <div><a href="<c:url value="/user/all" />">user</a></div>
    <div><a href="<c:url value="register" />">register</a></div>
</body>
</html>
