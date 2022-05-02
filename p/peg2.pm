package tau::peg2;


sub all
{
  my @ps = @_;
  sub
  {
    my ($m, $x, $y) = @_;
    my @r;
    defined(@r = $_->($m, $x, $y)) || return undef for @ps;
    @r;
  };
}


sub any
{
  my @ps = @_;
  sub
  {
    my ($m, $x, $y) = @_;
    my @r;
    defined(@r = $_->($m, $x, $y)) && return @r for @ps;
    undef;
  };
}


sub k
{
  my ($s, $news) = @_;
  my $n = length $s;
  sub
  {
    my ($m, $x, $y) = @_;
    my ($dx, $dy) = @{tau::map::news->{$news}};
    $m->sub($x, $y, $n) eq $s
        ? ($x + $dx * $n, $y + $dy * $n, $s)
        : undef;
  };
}


# TODO: define 2D regex format
sub re
{

}
