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


sub seq
{
  my @ps = @_;
  sub
  {
    my ($m, $x, $y) = @_;
    my @r;
    my @rvs;
    for (@ps)
    {
      return undef unless defined(@r = &$_($m, $x, $y));
      $x = shift @r;
      $y = shift @r;
      push @rvs, @r;
    }
    ($x, $y, @rvs);
  };
}


sub rep
{
  my ($p, $max) = (@_, -1 | 0);
  sub
  {
    my ($m, $x, $y) = @_;
    my @r;
    my @rvs;
    while (defined(@r = &$p($m, $x, $y)))
    {
      $x = shift @r;
      $y = shift @r;
      push @rvs, @r;
    }
    ($x, $y, @rvs);
  };
}


sub bow
{
  my ($news) = @_;
  my ($dx, $dy) = @{tau::map::news->{$news}};
  sub
  {
    my ($m, $x, $y) = @_;
    $m->has($x - $dx, $y - $dy) ? undef : ($x, $y);
  };
}
