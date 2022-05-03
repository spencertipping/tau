package tau::peg2;

use List::Util qw/max/;


sub all
{
  my @ps = @_;
  sub
  {
    my ($m, $x, $y) = @_;
    my @r;
    defined(@r = &$_($m, $x, $y)) || return undef for @ps;
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
    defined(@r = &$_($m, $x, $y)) && return @r for @ps;
    undef;
  };
}


sub multir(\%)
{
  my ($ps) = @_;
  sub
  {
    my ($m, $x, $y) = @_;
    my ($rx, $ry, @rv, %rc, %rv);
    while (my ($k, $p) = each %$ps)
    {
      if (defined(($rx, $ry, @rv) = &$p($m, $x, $y)))
      {
        $rv{$k} = [@rv];
        $rc{$k} = [$rx, $ry];
      }
    }
    (\%rc, \%rv);
  };
}


sub dspr(\%$)
{
  my ($ps, $news) = @_;
  my ($dx, $dy) = tau::map::news $news;
  sub
  {
    my ($m, $x, $y) = @_;
    my $ml = max map length, keys %$ps;
    my ($p, $rx, $ry, @rv);
    for my $l (map $ml - $_, 0..$ml-1)
    {
      my $k = $m->sub($x, $y, $news, $l);
      return ($rx, $ry, $k, @rv)
          if defined($p = $$ps{$k})
          && defined(($rx, $ry, @rv) = &$p($m, $x + $l*$dx, $y + $l*$dy));
    }
    undef;
  };
}


sub k
{
  my ($s, $news) = @_;
  my ($dx, $dy)  = tau::map::news $news;
  my $n          = length $s;
  sub
  {
    my ($m, $x, $y) = @_;
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


sub bow    # beginning of word
{
  my ($news) = @_;
  my ($dx, $dy) = tau::map::news $news;
  sub
  {
    my ($m, $x, $y) = @_;
    $m->has($x - $dx, $y - $dy) ? undef : ($x, $y);
  };
}


sub seek
{
  my ($news) = @_;
  sub
  {
    my ($m, $x, $y) = @_;
    my @r;
    defined(@r = $m->next($x, $y, $news)) ? @r : undef;
  };
}


sub is
{
  my ($c) = @_;
  $c = ord $c if $c !~ /^\d+$/;
  sub
  {
    my ($m, $x, $y) = @_;
    $m->at($x, $y) == $c ? ($x, $y) : undef;
  };
}


sub pmap(&$)
{
  my ($f, $p) = @_;
  sub
  {
    my ($m, $x, $y) = @_;
    my @r;
    return undef unless defined(($x, $y, @r) = &$p($m, $x, $y));
    ($x, $y, &$f(@r));
  };
}


sub pgrep(&$)
{
  my ($f, $p) = @_;
  sub
  {
    my ($m, $x, $y) = @_;
    my @r;
    return undef unless defined(($x, $y, @r) = &$p($m, $x, $y));
    &$f(@r) ? ($x, $y, @r) : undef;
  };
}


sub pflatmap(&$)
{
  my ($f, $p) = @_;
  sub
  {
    my ($m, $x, $y) = @_;
    my @r;
    return undef unless defined(($x, $y, @r) = &$p($m, $x, $y));
    my $p2;
    return undef unless defined($p2 = &$f($m, $x, $y, @r));
    &$p2($m, $x, $y);
  };
}


sub re
{
  my ($re, $news) = @_;
  my ($dx, $dy) = news $news;
  sub
  {
    my ($m, $x, $y) = @_;
    my $s = $m->sub($x, $y, $news);
    $s =~ s/^($re)//
        ? ($x + $dx * length $1, $y + $dy * length $1, @{^CAPTURE})
        : undef;
  };
}
