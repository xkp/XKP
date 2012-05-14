
on init()
{
    var series1 = myChart.series[0];
    series1.setData([10, 20, 0, 50, 3, -100]);

    myChart.ChartObject.addSeries({name: "Series2", data : [0, 10, 50, 20, 3, -10]});

    myGrid.data_source = 
    [
		{id:"1", invdate:"2007-10-01", name:"test",  note:"note",  amount:"200.00", tax:"10.00", total:"210.00"},
		{id:"2", invdate:"2007-10-02", name:"test2", note:"note2", amount:"300.00", tax:"20.00", total:"320.00"},
		{id:"3", invdate:"2007-09-01", name:"test3", note:"note3", amount:"400.00", tax:"30.00", total:"430.00"},
		{id:"4", invdate:"2007-10-04", name:"test",  note:"note",  amount:"200.00", tax:"10.00", total:"210.00"},
		{id:"5", invdate:"2007-10-05", name:"test2", note:"note2", amount:"300.00", tax:"20.00", total:"320.00"},
		{id:"6", invdate:"2007-09-06", name:"test3", note:"note3", amount:"400.00", tax:"30.00", total:"430.00"},
		{id:"7", invdate:"2007-10-04", name:"test",  note:"note",  amount:"200.00", tax:"10.00", total:"210.00"},
		{id:"8", invdate:"2007-10-03", name:"test2", note:"note2", amount:"300.00", tax:"20.00", total:"320.00"},
		{id:"9", invdate:"2007-09-01", name:"test3", note:"note3", amount:"400.00", tax:"30.00", total:"430.00"}
	];
}