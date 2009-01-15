#!/usr/cadc/misc/bin/perl     
#/*+
#************************************************************************
#****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#*
#* (c) 1998.                            (c) 1998.
#* National Research Council            Conseil national de recherches
#* Ottawa, Canada, K1A 0R6              Ottawa, Canada, K1A 0R6
#* All rights reserved                  Tous droits reserves
#*
#* NRC disclaims any warranties,        Le CNRC denie toute garantie
#* expressed, implied, or statu-        enoncee, implicite ou legale,
#* tory, of any kind with respect       de quelque nature que se soit,
#* to the software, including           concernant le logiciel, y com-
#* without limitation any war-          pris sans restriction toute
#* ranty of merchantability or          garantie de valeur marchande
#* fitness for a particular pur-        ou de pertinence pour un usage
#* pose.  NRC shall not be liable       particulier.  Le CNRC ne
#* in any event for any damages,        pourra en aucun cas etre tenu
#* whether direct or indirect,          responsable de tout dommage,
#* special or general, consequen-       direct ou indirect, particul-
#* tial or incidental, arising          ier ou general, accessoire ou
#* from the use of the software.        fortuit, resultant de l'utili-
#*                                      sation du logiciel.
#*
#************************************************************************
#*
#*   Script Name:       plot_growth.cgi
#*
#*   Purpose:
#       Displays an HTML form in which users can choose whether they 
#       would like to see the growth in file size of a specified database,
#       or if they would like to see the growth in file size and row size
#       of a specified table.
#       The information for the plot is found in either the table sizes_db 
#       or the table sizes_tbl depending on whether your plotting a 
#       database or table respecitvely.
#*
#*   Date               : Jan 11, 1999
#*
#*   SCCS data          : @(#)
#*      Module Name     : plot_growth.cgi
#*      Version Number  : 1.4
#*      Release Number  : 1
#*      Last Updated    : 02/08/99
#*
#*   Programmer         : David Delisle
#*
#*   Modification History:
#*
#****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
#************************************************************************
#-*/

$| = 1;

unshift ( @INC, "/usr/cadc/misc/lib/perl5/Sybase" );
require "sybperl.pl";

unshift ( @INC, "/usr/cadc/misc/perl/CGI.pm" );
use CGI qw( :standard );

$ENV{'PGPLOT_DIR'} = '/usr/cadc/misc/pgplot/';
use PGPLOT;

$ENV{'SYBASE'} = '/usr/cadc/sybase';
$CONFIG_DIR     = '/usr/external/httpd-CADC/scripts';


#
#   Get local configuration
#


if ( -f "$CONFIG_DIR/status-request.conf" )
{
    do "$CONFIG_DIR/status-request.conf" || die "Error reading configuration
	    file $CONFIG_DIR/status-request.conf\n";
}
else
{
    die "Configuration file '$CONFIG_DIR/status-request.conf' not found ? \n";
}


#
#   Initialize global variables
#


$graph_dir = "/usr/cadcdev/cadc/www/client/tmp/dbsizes";
$graph_url = "http://salish.dao.nrc.ca:8080/tmp/dbsizes";

$server = "SYBASE";
$db = "cadctemp";
$query = new CGI;

$database = $query->param( "database" ); 
$submit = $query->param( "submit" );
$table_info = $query->param( "table_info" );


#
#   Initialize $table and $owner variables.
#   Split $table_info on a match of 2 or more '_' because some table names
#   consist of single '_'.
#


( $table, $owner ) = split( /_{2,}/, $table_info );
if ( !$owner )
{
    $owner = "dbo";
}

$action = "http://salish.dao.nrc.ca:8080/cadcbin/dbsizes/plot_growth.cgi/";
$method = "POST";

$color2 = "#FF3333";
$color4 = "#3333FF";
$color5 = "#00CCFF";
$color6 = "#FF33CC";
$color8 = "#FF9900";
$color9 = "#66CC00";
$color11 = "#0099FF";
$color12 = "#9966CC";
$color14 = "#666666";


#
#   If image directory does not exist then create it.
#


if ( !( -e $graph_dir ) )
{
    use File::Path;
    mkpath( $graph_dir );
}


#
#  Connect to the database
#


$dbproc = &dblogin( $dbuser, $dbpswd, $dbserver );
if ( $dbproc == -1 )
{
    die "ERROR: Can't login into $server $db\n";
}

&dbuse( $db );


#
#  If no parameters were given, then return HTML page that lists all 
#  of the databases available.
#
#  Else if the value of submit is "Show tables" and a database 
#  value has been given, then return HTML page that lists all 
#  of the tables of a given database.
#
#  Else if the value of submit is "Plot graph" and a database 
#  value has been given, then plot the graph of the given database/table.
#
#  Else, correct name=value pairs not given so exit.
#


if ( !$query->param() || !$database )
{
    &database_pg;
}
elsif ( $submit eq "Show Tables" )
{
    &table_pg;
}
elsif ( $submit eq "Plot Graph" ) 
{
    &plot_graph;
}
else
{
    die "Incorrect name=value pairs given.\n";
}


#
#  Close the database connection.
#


&dbclose( $dbproc );
#
#+
########################################################################
#
#   Subroutine: database_pg	
#
#   Purpose:
#	Return HTML page to server that lists all of the databases
#       available.
#
#   Arguments:
#	None.
#
#   Return values:
#       None.
#
########################################################################
#-

sub	database_pg
{

    &doSql( "select distinct name=name
	    from sizes_db
	    order by name" );

    $index = 0;
    while ( %buffer = &dbnextrow( $dbproc, 1 ) )
    {
	$databases[$index] = $buffer{ 'name' };
	$index ++;
    }
    &dbcanquery();
    
   
    #
    #   Display HTML 
    #


    print $query->header;
    print $query->start_html( -Title=>'Databases at CADC', -BGCOLOR=>'#FFFFF' );
    
    print " <table align=left border=0>
		<tr>
		<td align=middle valign=middle>
		    <h1><font color=crimson>
		    The <font color=black>C</font>anadian 
		    <font color=black>A</font>stronomy 
		    <font color=black>D</font>ata 
		    <font color=black>C</font>entre (CADC)
		    </font></h1>
		</td>
		<td align=right valign=top>
		    <a target=_top href=http://www.nrc.ca>
			<img src=http://salish:8080/images/nrc.5.gif 
				border=0></a>
		</td> 
		</tr> 
		</table>  
		<br clear=left> 
		<hr size=2> 
	    
	    <center>
	    <H1> CADC Databases </H1>
	    </center>
	    <hr size=2> ";

    
    #
    #   Start form.
    #


    $size = @databases;
    if ( $size > 13 )
    {
	$size = 13;
    }
    
    print $query->p, $query->start_form( $method, $action );

    print $query->p,
	    $query->h3( "Please select a database" ,
	    $query->br,
	    $query->br,
	    $query->scrolling_list( -name=>'database', -"values"=>\@databases,
		    -default=>$databases[0], -size=>$size ) );

    print $query->p,
	    $query->submit( -name=>'submit', -value=>'Show Tables' ),
	    " ",
	    $query->submit( -name=>'submit', -value=>'Plot Graph' );
	    
    print $query->end_form;

    print $query->end_html;
}
#
#+
########################################################################
#
#   Subroutine:	table_pg
#
#   Purpose:
#	Return HTML page to server that lists all of the tables of a
#       given database..
#
#   Arguments:
#       None.
#
#   Return values:
#       None.
#
########################################################################
#-

sub	table_pg
{
    &doSql( "select distinct name=tblname, owner=owner
	    from sizes_tbl
	    where dbname='$database' and tblname!='datestamp' 
	    order by tblname, owner" );

    $index = 0;
    while ( %buffer = &dbnextrow( $dbproc, 1 ) )
    {
	$values[$index] = sprintf 
		( "%-35s %s", $buffer{ 'name' }, $buffer{ 'owner' } );
	$values[$index] =~ s/\s/_/g;
	$index ++;
    }
    &dbcanquery();


    #
    #   Display HTML 
    #


    print $query->header;
    print $query->start_html( -Title=>"$database tables at CADC", 
	    -BGCOLOR=>'#FFFFF' );
    
    print " <table align=left border=0>
		<tr>
		<td align=middle valign=middle>
		    <h1><font color=crimson>
		    The <font color=black>C</font>anadian 
		    <font color=black>A</font>stronomy 
		    <font color=black>D</font>ata 
		    <font color=black>C</font>entre (CADC)
		    </font></h1>
		</td>
		<td align=right valign=top>
		    <a target=_top href=http://www.nrc.ca>
			<img src=http://salish:8080/images/nrc.5.gif
				border=0></a>
		</td> 
		</tr> 
	    </table>  
	    <br clear=left> 
	    <hr size=2> 
	    
	    <center>
	    <H1> CADC $database database tables </H1>
	    </center>
	    <hr size=2> ";


    #
    #   Start form.
    #


    $size = @values;
    if ( $size > 13 )
    {
	$size = 13;
    }
    print $query->p, $query->start_form( $method, $action );
    
    print " <h3>Please select a table</h3>
            <pre>Table Name                                    Table Owner</pre>
	    <font size=7 face='monospace'> ";
    
    print $query->scrolling_list( -name=>'table_info', -"values"=>\@values,
		    -default=>$values[0], -size=>$size );
    
    print " </font> 
	    <h5>(if no table is selected then graph of $database database
		    will be plotted)</h5> ";
    
    
    print $query->hidden( -name=>'database', -default=>'$database' ); 
    
    print $query->p,
	    $query->submit( -name=>'submit', -value=>'Plot Graph' );
	    
    print $query->end_form;

    print $query->end_html;
}
#
#+
########################################################################
#
#   Subroutine: plot_graph	
#
#   Purpose:
#	Plot a graph of a database/table that shows the growth of the 
#       database/table over time.
#
#   Arguments:
#       None.
#
#   Return values:
#       None.
#
########################################################################
#-

sub     plot_graph	
{


    #
    #   Set current date, date a year ago and 
    #   array of completed dates for the given database.
    #


    &set_date;


    #
    #   If their is a value for table, then plot a table.
    #   Else, plot a database.
    #


    if ( $table )
    {
	&set_tbl_coord; 

	&plot_tbl_size;
	&plot_tbl_rows;
    }
    else
    {
	&set_db_size_coord; 
	&plot_db_size;

	&set_db_tables_coord;
	&plot_db_tables;
    }


    #
    #   Display file size plot.
    #   If a table value exists then also display row size plot.
    #


    &display_plot;
}
#
#+
########################################################################
#
#   Subroutine:	set_date
#
#   Purpose:
#	Sets the current date, date a year ago, array of completed dates, 
#       and number of points in the current plot, for the given database.
#
#       The current date and date a year ago converted to get rid of 
#       minutes and seconds is set to be the label of the x-axis in the 
#       current plot.
#
#       The completed dates converted to days from today initialize
#       the x values in the current plot.
#       Sets the number of points in the current plot.
#
#   Arguments:
#	None.
#
#   Return values:
#       None.
#
########################################################################
#-

sub	set_date
{
    
    
    #
    #   Set current date and date a year ago
    #

    
    &doSql( "select present=getdate(), past=dateadd(yy, -1, getdate())" );
    %buffer = &dbnextrow( $dbproc, 1 );
    $date_now = $buffer{ 'present' };
    $date_then = $buffer{ 'past' };
    &dbcanquery();


    #
    #   Change date format of current date and date a year ago to get rid 
    #   of minutes and seconds.
    #   These new dates are used for labeling the x-axis of the current plot. 
    #


    &doSql( "select present=convert( char(12), '$date_now', 107 ),
	    past=convert( char(12), '$date_then', 107 )" );
    %buffer = &dbnextrow( $dbproc, 1 );
    $date_now_label = $buffer{ 'present' };
    $date_then_label = $buffer{ 'past' };
    &dbcanquery();


    #
    #   Set array of completed dates
    #


    &doSql( "select date=date, day=datediff(day, '$date_now', date)
	    from sizes_tbl
	    where dbname='$database' and tblname='datestamp' 
		    and date >= '$date_then' and date <= '$date_now'
	    order by date" );

    $index = 0;
    while ( %buffer = &dbnextrow( $dbproc, 1 ) )
    {
	$dates[$index] = $buffer{ 'date' };
	$x[$index] = $buffer{ 'day' };
	$index++;
    }
    &dbcanquery();
    
    $points = @x;
}
#
#+
########################################################################
#
#   Subroutine: set_tbl_coord 
#
#   Purpose:
#       Sets arrays of the y coordinates of the 
#       row size, reserved size, data size, and index size
#       of a given table.
#       Sizes are stored in megabytes rather than kilobytes.
#       If a table did not exist on a completed date, then
#       it's y coordinate for that date is set to 0.
#
#   Arguments:
#       None.
#
#   Return values:
#       None. 
#
########################################################################
#-

sub     set_tbl_coord
{
    $index = 0;
    foreach $date ( @dates )
    {
	&doSql( "select row=row_sz,
		reserved=reserved_sz, data=data_sz,
		index_sz=index_sz
		from sizes_tbl
		where dbname='$database' and tblname='$table'
		and owner='$owner' and date='$date'
		order by date" );
	
	while ( %buffer = &dbnextrow( $dbproc, 1 ) ) 
	{
	    $row_y[$index] = $buffer{ 'row' };
    
	    $reserved_y[$index] = $buffer{ 'reserved' };
	    $reserved_y[$index] /= 1024;
    
	    $data_y[$index] = $buffer{ 'data' };
	    $data_y[$index] /= 1024;
    
	    $index_y[$index] = $buffer{ 'index_sz' };
	    $index_y[$index] /= 1024;
	}		    
	$num_rows = &DBCOUNT;
	&dbcanquery();


	#
	#   If no rows returned from above query then table
	#   did not exist on this date.
	#   Set y values to 0.
	#


	if ( $num_rows == 0 )
	{
	    $row_y[$index] = 0;
	    $reserved_y[$index] = 0;
	    $data_y[$index] = 0;
	    $index_y[$index] = 0;
	}
	$index++;
    }
}
#
#+
########################################################################
#
#   Subroutine: set_db_size_coord 
#
#   Purpose:
#       Sets arrays of the y coordinates of the 
#       total size, reserved size, data size, and index size
#       of a given database.
#       Sizes are stored in megabytes rather than kilobytes.
#       If a database did not exist on a completed date, then
#       it's y coordinate for that date is set to 0.
#
#   Arguments:
#       None.
#
#   Return values:
#       None. 
#
########################################################################
#-
sub     set_db_size_coord
{
    $index = 0;
    foreach $date ( @dates )
    {
	&doSql( "select total=database_sz, 
		reserved=reserved_sz, data=data_sz,
		index_sz=index_sz
		from sizes_db
		where name='$database' and date='$date'
		order by date" );
	
	while ( %buffer = &dbnextrow( $dbproc, 1 ) ) 
	{
	    $total_y[$index] = $buffer{ 'total' };
	    $total_y[$index] /= 1024;
	    
	    $reserved_y[$index] = $buffer{ 'reserved' };
	    $reserved_y[$index] /= 1024;
    
	    $data_y[$index] = $buffer{ 'data' };
	    $data_y[$index] /= 1024;
    
	    $index_y[$index] = $buffer{ 'index_sz' };
	    $index_y[$index] /= 1024;
	}		    
	$num_rows = &DBCOUNT;
	&dbcanquery();


	#
	#   If no rows returned from above query then database
	#   did not exist on this date.
	#   Set y values to 0.
	#


	if ( $num_rows == 0 )
	{
	    $total_y[$index] = 0;
	    $reserved_y[$index] = 0;
	    $data_y[$index] = 0;
	    $index_y[$index] = 0;
	}
	$index++;
    }
}
#
#+
########################################################################
#
#   Subroutine: set_db_tables_coord 
#
#   Purpose:
#       Sets arrays of the y coordinates of the tables 
#       of a given database.
#       Sizes are stored in megabytes rather than kilobytes.
#       If a table did not exist on a completed date, then
#       it's y coordinate for that date is set to 0.
#
#   Arguments:
#       None.
#
#   Return values:
#       None. 
#
########################################################################
#-

sub     set_db_tables_coord
{


    #
    #   Get table names and their corresponding owner,
    #   not including datestamp table.
    #
 
 
    &doSql( "select name=tblname, owner=owner
	    from sizes_tbl
	    where dbname='$database' and tblname!='datestamp' 
	    group by tblname, owner
	    order by max(reserved_sz)" );

    $index = 0;
    while ( %buffer = &dbnextrow( $dbproc, 1 ) )
    {
	$tables[$index] = $buffer{ 'name' }; 
	$owners[$index] = $buffer{ 'owner' };
	$index ++;
    }
    &dbcanquery();


    #
    #   For each table get coordinates.
    #


    $num_tables = @tables;
    for ( my ($table) = 0; $table < $num_tables; $table++ )
    {
	$index = 0;
	foreach $date ( @dates )
	{
	    &doSql( "select size=reserved_sz 
		    from sizes_tbl
		    where dbname='$database' and tblname='$tables[$table]'   
			    and owner='$owners[$table]' 
			    and date='$date'" );

	    while ( %buffer = &dbnextrow( $dbproc, 1 ) ) 
	    {
		${"ytable$table"}[$index] = $buffer{ 'size' };
		${"ytable$table"}[$index] /= 1024;
	    }		    
	    $num_rows = &DBCOUNT;
	    &dbcanquery();
	

	    #
	    #   If no rows returned form above query then table 
	    #   did not exist on this date.
	    #   Set y values to 0.
	    #

	    
	    if ( $num_rows == 0 )
	    {
		${"ytable$table"}[$index] = 0;
	    }
	    $index++;
	}	


	#
	#   If the first table, then initialize @ytable-1 to all 0's.
	#   Else, add the last tables y values to the current table.
	#


	$last_table = $table - 1;
	
	if ( $table == 0 )
	{
	    for ( $i = 0; $i < $points; $i++ )
	    {
		${"ytable${last_table}"}[$i] = 0;
	    }
	}
	else
	{
	    for ( $i=0; $i < $points; $i++ )
	    {
		${"ytable$table"}[$i] += ${"ytable${last_table}"}[$i];
	    }
	}
    }
}
#
#+
########################################################################
#
#   Subroutine: plot_tbl_size 
#
#   Purpose:
#       Plots the coordinates of the different file size types for the given
#       table and saves them in the file tbl_size.gif.
#
#   Arguments:
#       None
#
#   Return values:
#       None.
#
########################################################################
#-

sub     plot_tbl_size
{
    $tbl_size_file = "$table\_size.gif";
    $device = "\'\"$graph_dir/$tbl_size_file\"/GIF\'";
    eval 'pgbegin( 0, '.$device.', 1, 1 )';
    pgpap( 6.5, 1 );
	
    $y_max = 0;
    foreach $value ( @reserved_y )
    {
	if ( $value > $y_max )
	{
	    $y_max = $value;
	}
    }
    $max = $y_max * 1.10;
    $min = $y_max * -0.10;
   
   
    #
    #   Set background color to white and foreground color to black
    #


    pgscr( 0, 1, 1, 1 );
    pgscr( 1, 0, 0, 0 );
    
    pgsch( 1.6 );           
    pgenv( -401.5, 36.5, $min, $max, 0, -2 );
   
    pgsvp( .12, .94, .12, .94 );
    pgsch( .7 );
    pgbox( BNTS, 73, 0, BNTS, 0, 0 );
    pgbox( C, 0, 0, C, 0, 0 );

    pgscf(2);
    pgsch(1.3);
    pglabel( "$date_then_label - $date_now_label", 'Megabytes', "" );
   
    pgslw( 6 );
   
    pgsci(4);
    pgline( $points, \@x, \@reserved_y );
	
    pgsci(2);
    pgline( $points, \@x, \@data_y );
	
    pgsci(6);
    pgline( $points, \@x, \@index_y );

    pgend;
}

#
#+
########################################################################
#
#   Subroutine: plot_tbl_rows 
#
#   Purpose:
#       Plots the coordinates of the rows for the given table and saves 
#       them in the file tbl_rows.gif.
#
#   Arguments:
#       None
#
#   Return values:
#       None.
#
########################################################################
#-

sub     plot_tbl_rows
{
    $tbl_rows_file = "$table\_rows.gif";
    $device = "\'\"$graph_dir/$tbl_rows_file\"/GIF\'";
    
    eval 'pgbegin( 0, '.$device.', 1, 1 )';
    pgpap( 6.5, 1 );

    $row_max = 0;
    foreach $value ( @row_y )
    {
	if ( $value > $row_max )
	{
	    $row_max = $value;
	}
    }
    $max = $row_max * 1.10;
    $min = $row_max * -0.10;
   
   
    #
    #   Set background color to white and foreground color to black
    #


    pgscr( 0, 1, 1, 1 );
    pgscr( 1, 0, 0, 0 );
    
    pgsch( 1.6 );           
    pgenv( -401.5, 36.5, $min, $max, 0, -2 );
   
    pgsvp( .12, .94, .12, .94 );
    pgsch( .7 );
    pgbox( BNTS, 73, 0, BNTS, 0, 0 );
    pgbox( C, 0, 0, C, 0, 0 );

    pgscf(2);
    pgsch(1.3);
    pglabel( "$date_then_label - $date_now_label", 'Number of Rows', "" );

    pgslw( 6 );

    pgsci(4);
    pgline( $points, \@x, \@row_y );
    
    pgend;
}
#
#+
########################################################################
#
#   Subroutine: plot_db_size 
#
#   Purpose:
#       Plots the coordinates of the different file size types.  
#
#   Arguments:
#       None
#
#   Return values:
#       None.
#
########################################################################
#-

sub     plot_db_size
{
    $db_size_file = "$database\_size.gif";
    $device = "\'\"$graph_dir/$db_size_file\"/GIF\'";
    
    eval 'pgbegin( 0, '.$device.', 1, 1 )';
    pgpap( 6.5, 1 );

    $y_max = 0;
    foreach $value ( @total_y )
    {
	if ( $value > $y_max )
	{
	    $y_max = $value;
	}
    }
    $max = $y_max * 1.10;
    $min = $y_max * -0.10;
   
   
    #
    #   Set background color to white and foreground color to black
    #


    pgscr( 0, 1, 1, 1 );
    pgscr( 1, 0, 0, 0 );
    
    pgsch( 1.6 );           
    pgenv( -401.5, 36.5, $min, $max, 0, -2 );
   
    pgsvp( .12, .94, .12, .94 );
    pgsch( .7 );
    pgbox( BNTS, 73, 0, BNTS, 0, 0 );
    pgbox( C, 0, 0, C, 0, 0 );

    pgscf(2);
    pgsch(1.3);
    pglabel( "$date_then_label - $date_now_label", 'Megabytes', "" );
   
    pgslw( 6 );
   
    pgsci(4);
    pgline( $points, \@x, \@total_y );
    
    pgsci(2);
    pgline( $points, \@x, \@reserved_y );
	
    pgsci(6);
    pgline( $points, \@x, \@data_y );
	
    pgsci(8);
    pgline( $points, \@x, \@index_y );

    pgend;
}
#
#+
########################################################################
#
#   Subroutine: plot_db_tables 
#
#   Purpose:
#       Plots the coordinates of the different tables of a given database 
#       and saves them in the file db_tables.gif.
#
#   Arguments:
#       None
#
#   Return values:
#       None.
#
########################################################################
#-

sub     plot_db_tables
{
    
    
    #
    #   Initialize x values and number of points for plot
    #

    
    @poly_x = @x;
    push( @poly_x, reverse( @x ) );
    $poly_points = $points * 2; 
   
    $db_tables_file = "$database\_tables.gif";
    $device = "\'\"$graph_dir/$db_tables_file\"/GIF\'";
    
    eval 'pgbegin( 0, '.$device.', 1, 1 )';
    pgpap( 6.5, 1 );

    $y_max = 0;
    foreach $value ( @total_y )
    {
	if ( $value > $y_max )
	{
	    $y_max = $value;
	}
    }
    $max = $y_max * 1.10;
    $min = $y_max * -0.10;
   
   
    #
    #   Set background color to white and foreground color to black
    #


    pgscr( 0, 1, 1, 1 );
    pgscr( 1, 0, 0, 0 );

    pgsch( 1.6 );           
    pgenv( -401.5, 36.5, $min, $max, 0, -2 );
    
    pgsvp( .12, .94, .12, .94 );
    pgsch( .7 );
    pgbox( BNTS, 73, 0, BNTS, 0, 0 );
    pgbox( C, 0, 0, C, 0, 0 );
	
    pgscf(2);
    pgsch(1.3);
    pglabel( "$date_then_label - $date_now_label", 'Megabytes', "" );
	
    $color = 2; 
    for ( my ($table) = 0; $table < $num_tables; $table++ )
    {
	pgsci($color);
        	
	@poly_y = @{"ytable$table"};
	
	$last_table = $table - 1;
	push( @poly_y, reverse( @{"ytable${last_table}"} ) );
	
	pgpoly( $poly_points, \@poly_x, \@poly_y );	
   
	if ( $table == $num_tables - 1 )
	{
	    $last_color = $color;
	}
	
	$color ++;
	
	if ( $color == 3 )
	{
	    $color = 4;
	}
	elsif ( $color == 5 )
	{
	    $color = 6;
	}
	elsif ( $color == 7 )
	{
	    $color = 8;
	}
	elsif ( $color == 10 )
	{
	    $color = 11;
	}
	elsif ( $color == 13 )
	{
	    $color = 14;
	}
	elsif ( $color == 15 )
	{
	    $color = 2;
	}
    }
    pgend;
}
#
#+
########################################################################
#
#   Subroutine: display_plot	
#
#   Purpose:
#	Displays plots for a database/table.
#
#   Arguments:
#	None.
#
#   Return values:
#       None.
#
########################################################################
#-

sub	display_plot
{
    print $query->header;
    print $query->start_html( -Title=>'File Size Growth Graph', 
	    -BGCOLOR=>'#FFFFF' );
    
    print " <table align=left border=0>
		<tr>
		<td align=middle valign=middle>
		    <h1><font color=crimson>
		    The <font color=black>C</font>anadian 
		    <font color=black>A</font>stronomy 
		    <font color=black>D</font>ata 
		    <font color=black>C</font>entre (CADC)
		    </font></h1>
		</td>
		<td align=right valign=top>
		    <a target=_top href=http://www.nrc.ca>
			<img src=http://salish:8080/images/nrc.5.gif 
				border=0></a>
		</td> 
		</tr> 
	    </table>  
	    <br clear=left> 
	    <hr size=2> "; 

    if ( $table )
    {
	print " <H1>Database: <em>$database</em><br>
		    Table:    <em>$table</em><br>
		    Owner:    <em>$owner</em><br></H1>
		<hr size=2>
		
		<br>
		<br>
		
		<center>
		<H2><em>Overview</em></H2>
		<img src=$graph_url/$tbl_size_file alt='graph' align='left'>
	
		<H3>
		
		<div align=right>
		<font size=3 color=#000000>
		LEGEND
		</font>
		<br>
		<br>

		<font size=2 color=$color4>
		Reserved Size 
		</font>
		<br>
	
		<font size=2 color=$color2>
		Data Size 
		</font>
		<br>
	
		<font size=2 color=$color6>
		Index Size 
		</font>
		<br>

		</div>
	
		</H3>
		
		<br clear=left>
		<hr size=2>
		
		<br>
		
		<H2><em>Rows</em></H2>
		</center>
		<img src=$graph_url/$tbl_rows_file alt='graph' align='left'>

		<H3>
		
		<div align=right>
		<font size=3 color=#000000>
		LEGEND
		</font>
		<br>
		<br>

		<font size=2 color=$color4>
		Row Size
		</font>
		<br>
		
		</div>
		</H3>

		<br clear=left>
		<hr size=2> ";
    }
    else
    {
	print " <H1> Database: <em>$database</em></H1>
		<hr size=2>

		<br>
		<br>
		
		<center>
		<H2><em>Overview</em></H2>
		<img src=$graph_url/$db_size_file alt='graph' align='left'>
	
		<H3>
		
		<div align=right>
		<font size=3 color=#000000>
		LEGEND
		</font>
		<br>
		<br>

		<font size=2 color=$color4>
		Total Size 
		</font>
		<br>
	
		<font size=2 color=$color2>
		Reserved Size 
		</font>
		<br>
	
		<font size=2 color=$color6>
		Data Size 
		</font>
		<br>
	
		<font size=2 color=$color8>
		Index Size 
		</font>
		<br>

		</div>
		
		</H3>
		
		<br clear=left>
		<hr size=2> 
		
		<br>
		
		<H2><em>Tables</em></H2>
		</center>
		<img src=$graph_url/$db_tables_file alt='graph' align='left'>
	
		<H3>
		
		<div align=right>
		<font size=3 color=#000000>
		<b><em>LEGEND</em></b>
		</font>
		<br>
		<br> ";
	

	#
	#   Display legend for table.gif plot
	#

	
	$color = $last_color;
	for ( $index = $num_tables - 1; $index >= 0; $index-- )
	{
	    $value = ${"color$color"};
	    print " <a href='./plot_growth.cgi/?database=$database&submit=Plot+Graph&table_info=$tables[$index]__$owners[$index]'>
		    <font size=2 color=$value>
		    $tables[$index]
		    </font>
		    </a>
		    <br> ";

	    $color --;

	    if ( $color == 13 )
	    {
		$color = 12;
	    }
	    elsif ( $color == 10 )
	    {
		$color = 9;
	    }
	    elsif ( $color == 7 )
	    {
		$color = 6;
	    }
	    elsif ( $color == 5 )
	    {
		$color = 4;
	    }
	    elsif ( $color == 3 )
	    {
		$color = 2;
	    }
	    elsif ( $color == 1 )
	    {
		$color = 14;
	    }
	}
		
	print " </div>
		</H3>

		<br clear=left>
		<hr size=2> ";
    }
   
    print $query->end_html;
}
#
#+
########################################################################
#
#   Subroutine: doSql
#
#   Purpose:
#       Executes an SQL query.
#
#   Arguments:
#       @_              : Query buffer
#
#   Return values:
#       None.
#
########################################################################
#-

sub     doSql
{
    local( $sql ) = @_;

    &dbcmd( $sql ) || die "Error: in dbcmd.\n" ;
    &dbsqlexec || die "Error: in dbsqlexec.\n" ;
    &dbresults;
}



