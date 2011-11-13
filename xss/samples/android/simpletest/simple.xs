on btnFullName.click()
{
	string fullname = edtFirstName.text + " " + edtLastName.text;
	
	if(fullname != " ")
		lblFullName.caption = "Your full name is " + fullname;
	else
		lblFullName.caption = "Your full name is EMPTY!";
	
	edtContainer.append(lblFullName.caption);
}
