on btnFullName.click()
{
	string fullname = edtFirstName.text + " " + edtLastName.text;
	
	if(fullname != " ")
		lblFullName.caption = "Your fullname is " + fullname;
	else
		lblFullName.caption = "Your fullname is EMPTY!";
}
