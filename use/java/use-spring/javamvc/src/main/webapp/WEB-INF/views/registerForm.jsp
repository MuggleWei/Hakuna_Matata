<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c" %>
<%@ page session="false" %>
<html>
<head>
    <title>register</title>
    <link rel="stylesheet"
          type="text/css"
          href="<c:url value="/resources/style.css" />" />
</head>
<body>
<h1>Register</h1>

<form method="POST">
    user name: <input type="text" name="userName" /><br/>
    password: <input type="password" name="password" /><br/>
    email: <input type="email" name="email" /><br/>
    phone: <input type="text" name="phoneNumber" /><br/>
    <input type="submit" value="register" />
</form>

</body>
</html>
